#include <linux/skbuff.h>
#include <linux/hdreg.h>
#include <linux/mempool.h>
#include <linux/debugfs.h>
#include <linux/relay.h>
#include "aoe.h"

#define SUBBUF_SIZE 262144
#define N_SUBBUFS 8

/* aoe module channel for ad hoc messages */
static struct rchan *chan;

static struct timeval start;

/* per-aoedev debugging support */
struct ddbg {
	struct rchan *rttchan;
	atomic_t rttseq;
};

static char buf[PAGE_SIZE];
static DEFINE_SPINLOCK(buflock);

/* required callbacks for using debugfs as host fs for relay */
static struct dentry *
cfile(const char *filename,
      struct dentry *parent,
      int mode,
      struct rchan_buf *buf,
      int *is_global)
{
	return debugfs_create_file(filename, mode, parent, buf,
				   &relay_file_operations);
}
static int
rfile(struct dentry *dentry)
{
	debugfs_remove(dentry);

	return 0;
}
static int
subbuf_start(struct rchan_buf *buf,
	     void *subbuf,
	     void *prev_subbuf,
	     size_t prev_padding)
{
	return 1;		/* overwrite mode */
}
static struct rchan_callbacks cb = {
	.subbuf_start = subbuf_start,
	.create_buf_file = cfile,
	.remove_buf_file = rfile,
};

/* Only call xrelay_open from code that can sleep. */
static struct rchan *
xrelay_open(const char *base_fnam,
	    struct dentry *parent,
	    size_t subbuf_size,
	    size_t n_subbufs)
{
	struct rchan *c;

	c = relay_open(base_fnam, parent, subbuf_size, n_subbufs, &cb, NULL);

	if (!c)
		printk(KERN_ERR
		       "aoe: could not create relay channel %s\n",
		       base_fnam);
	else
		printk(KERN_INFO
		       "aoe: relay channel %s ready\n", base_fnam);

	return c;
}

void __init
aoedbg_init(void)
{
	enum {
		AOE_CHAN_SUBBUF_SIZE = 1024 * 64,
		AOE_CHAN_N_SUBBUFS = 4,
	};
	
	if (!AOEDBG_ACTIVE)
		return;

	chan = xrelay_open("aoe", NULL,
			   AOE_CHAN_SUBBUF_SIZE, AOE_CHAN_N_SUBBUFS);

	do_gettimeofday(&start);
}

static void
aoedbg_write(const void *data, size_t length)
{
	if (!chan)
		return;

	relay_write(chan, data, length);
}

/* formatted printing on the aoe-module generic channel */
void
aoedbg_print(char *fmt, ...)
{
	va_list va;
	unsigned long flags;
	int n;

	if (!chan)
		return;

	va_start(va, fmt);
	spin_lock_irqsave(&buflock, flags);
	n = vsnprintf(buf, sizeof buf - 1, fmt, va);
	spin_unlock_irqrestore(&buflock, flags);
	aoedbg_write(buf, n);
}

static int
rttstart(struct aoedev *d)
{
	struct ddbg *dbg;
	struct rchan *c;
	char name[32] = { 0, };

	snprintf(name, sizeof name - 1,
		 "aoe-e%ld.%d-rtts",
		 d->aoemajor, d->aoeminor);
	c = xrelay_open(name, NULL, SUBBUF_SIZE, N_SUBBUFS);
	if (!c)
		return -1;

	dbg = d->dbg;
	dbg->rttchan = c;
	atomic_set(&dbg->rttseq, 0);
	
	return 0;
}

static void
rttstop(struct ddbg *dbg)
{
	if (!dbg->rttchan)
		return;

	relay_flush(dbg->rttchan);
	relay_close(dbg->rttchan);
	dbg->rttchan = NULL;
}

/* called from code that can sleep */
void
aoedbg_devinit(struct aoedev *d)
{
	struct ddbg *dbg;
	unsigned long flags;

	if (!AOEDBG_ACTIVE)
		return;
	
	dbg = aoe_kcalloc(1, sizeof *dbg, GFP_KERNEL);
	if (!dbg) {
		printk(KERN_ERR
		       "aoe: could not allocate memory to debug e%ld.%d\n",
		       d->aoemajor, d->aoeminor);
		return;
	}
	spin_lock_irqsave(&d->lock, flags);
	if (d->dbg || (d->flags & DEVFL_TKILL)) {
		spin_unlock_irqrestore(&d->lock, flags);
		kfree(dbg);
		return;
	}
	d->dbg = dbg;
	spin_unlock_irqrestore(&d->lock, flags);
	
	if (!rttstart(d))
		return;
	d->dbg = NULL;
	kfree(dbg);
}

void
aoedbg_rtt(struct aoedev *d, struct aoe_hdr *h, struct frame *f, int rtt)
{
	struct timeval now;
	enum recflags {
		R_ORPHAN = 1,
		R_WRITE = 1 << 1,
	};
	struct ddbg *dbg;
	struct aoe_atahdr *ah;
	struct record {
		u32 seqno;
		u32 rtt;
		u32 reltime;
		u32 rttavg;
		u32 rttdev;
		u8 send_nout;
		u8 rcv_nout;
		u8 ssthresh;
		u8 cwnd;
		u8 lba0;
		u8 lba1;
		u8 lba2;
		u8 lba3;
		u8 lba4;
		u8 lba5;
		u8 flags;
		u8 remote[6];
		u8 local[6];
	} __attribute__ ((packed)) r = { 0, };

	dbg = d->dbg;

	if (!dbg || !dbg->rttchan)
		return;

	ah = (struct aoe_atahdr *)(h + 1);
	r.seqno = (u32) atomic_inc_return(&dbg->rttseq);
	r.rtt = (u32) rtt;
	do_gettimeofday(&now);
	r.reltime = now.tv_usec - start.tv_usec;
	r.reltime += (now.tv_sec - start.tv_sec) * USEC_PER_SEC;
	r.rttavg = d->rttavg >> RTTSCALE;
	r.rttdev = d->rttdev >> RTTDSCALE;
	if (!f) {
		r.flags |= R_ORPHAN;
	} else {
		r.send_nout = f->nout;
		r.rcv_nout = f->t->nout;
		r.ssthresh = f->t->ssthresh;
		r.cwnd = f->t->maxout;
	}
	r.lba0 = ah->lba0;
	r.lba1 = ah->lba1;
	r.lba2 = ah->lba2;
	r.lba3 = ah->lba3;
	r.lba4 = ah->lba4;
	r.lba5 = ah->lba5;
	r.flags |= !!(ah->aflags & AOEAFL_WRITE);
	memmove(r.remote, h->src, sizeof r.remote);
	memmove(r.local, h->dst, sizeof r.local);
	
	relay_write(dbg->rttchan, &r, sizeof r);
}

void
aoedbg_undbg(struct aoedev *d)
{
	struct ddbg *dbg;

	dbg = (struct ddbg *) d->dbg;

	if (!dbg)
		return;

	rttstop(dbg);
	kfree(dbg);
	d->dbg = NULL;
}

void
aoedbg_exit(void)
{
	if (chan)
		relay_close(chan);
}
