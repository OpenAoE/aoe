/* Copyright Coraid, Inc. 2010.  All Rights Reserved. */

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <poll.h>
#include <errno.h>

#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define	nelem(x)	(sizeof(x)/sizeof((x)[0]))
#define nil NULL
#define vprintf(...) if (qflag) ; else fprintf(stderr, __VA_ARGS__)
#define mintu(x) ((x) > 60 ? x : 60)

typedef unsigned char uchar;
typedef unsigned long ulong;

typedef struct Aoe Aoe;
typedef struct Qc Qc;
typedef struct Ata Ata;
typedef struct Lun Lun;
typedef struct Eth Eth;
typedef struct Targ Targ;
typedef struct Mac Mac;

struct Aoe {
	uchar dst[6];
	uchar src[6];
	uchar type[2];
	uchar flags;
	uchar error;
	uchar major[2];
	uchar minor;
	uchar cmd;
	uchar tag[4];
};

struct Qc {
	Aoe h;
	uchar bufcnt[2];
	uchar firmware[2];
	uchar scnt;
	uchar vercmd;
	uchar len[2];
//	uchar data[1024];
};


struct Ata {
	Aoe h;
	uchar aflag;
	uchar err;
	uchar scnt;
	uchar cmd;
	uchar lba[6];
	uchar res[2];
};

struct Lun {
	Lun *next;
	int state;
	char ea[6];
	int major;
	int minor;
	int nsect;
	int maxsect;
	int id;
};

struct Eth {
	Lun *luns;
	int fd;
	char *name;
	char ea[6];
	int mtu;
	int up;
	uchar pkt[16*1024];
};

struct Targ {
	Targ *next;
	int major;
	int minor;
};

struct Mac {
	Mac *next;
	char ea[6];
};

enum {
	Neth= 16,
	Nstack= 16*1024,
	Nws= 5,

	Lnew= 0,
	Lprobe,

	Arsp= (1<<3),
	Aerr= (1<<2),

	Cata= 0,
	Cqc= 1,

	ETaoe= 0x88a2,
};

int ethlist(char **, int);
int ethopen(Eth *);
void *jcheck(void *);
int jinput(Eth *);
Lun *findlun(Eth *e, Aoe *a);
void jprobe(Eth *e, Lun *lun);
void printlist(void);
void printsancheck(void);
ulong nhgetl(uchar *);
ushort nhgets(uchar *);
void hnputs(uchar *, ushort);
void hnputl(uchar *, ulong);
void *mallocz(int);
void inserttarg(int, int);
void insertmac(Mac **, char *);
void sancheck(int, int);
void ifsummary(void);
int ifup(char *);
void inserteth(char **, int, char *);
char *getpciid(char *, int, char *);

Eth eth[Neth];
int waitsecs = Nws;
pthread_t threads[Neth];
Targ *targlist;

int
main(int argc, char **argv)
{
	int n, i;
	char *ethnames[Neth];
	Eth *e;

	memset(ethnames, 0, sizeof ethnames);
	printf("Probing...");
	fflush(0);
	n = ethlist(ethnames, nelem(ethnames));
	for (i=0; i<n; i++) {
		e = &eth[i];
		e->name = ethnames[i];
		e->up = ifup(ethnames[i]);
		if (pthread_create(&threads[i], 0, jcheck, e)) {
			fprintf(stderr, "pthread_create failed.\n");
			break;
		}
	}
	n = i;
	for (i=0; i<n; i++)
		pthread_join(threads[i], 0);
	printf("done.\n");
	printsancheck();
	if (argc == 2 && strcmp(argv[1], "-v") == 0)
		printlist();
	return 0;
}

char *
cea(char *op, char *ea)
{
	int i;
	char *hex = "0123456789abcdef", *p;

	p = op;
	for (i=0; i<6; i++) {
		*p++ = hex[((ea[i] >> 4) & 0xf)];
		*p++ = hex[ea[i] & 0xf];
	}
	*p = 0;
	return op;
}

void
printlist(void)
{
	Eth *e;
	Lun *lun;
	char mac[13];

	for (e=eth; e->name; e++) {
		printf("%s:\n", e->name);
		for (lun=e->luns; lun; lun=lun->next)
			printf("e%d.%d %s %d\n", lun->major, lun->minor, cea(mac, lun->ea), lun->maxsect);
		printf("\n");
	}
}

void
timewait(int secs)  /* arrange for a sig_alarm signal after `secs' seconds */
{
    struct sigaction sa;
	void catch(int);

    memset(&sa, 0, sizeof sa);
    sa.sa_handler = catch;
    sa.sa_flags = SA_RESETHAND;
    sigaction(SIGALRM, &sa, NULL);
    alarm(secs);
}

int
discover(Eth *e)
{
	Aoe *a;
	Qc *q;

	memset(e->pkt, 0, sizeof e->pkt);
	a = (Aoe *) e->pkt;
	memset(a->dst, 0xff, 6);
	memmove(a->src, e->ea, 6);
	hnputs(a->type, ETaoe);
	hnputl(a->tag, 1<<31);
	hnputs(a->major, 0xffff);
	a->minor = 0xff;
	a->cmd = Cqc;
	a->flags = 0x10;
	if (write(e->fd, a, mintu(sizeof *q)) <= 0)
		return -1;
	return 0;
}

void *
jcheck(void *v)
{
	Eth *e = v;
	int n;
	time_t t, nt;
	struct pollfd pd;

	if (ethopen(e) < 0)
		return 0;
	if (discover(e) < 0) {
		fprintf(stderr, "skipping %s, discover failure: %s\n", e->name, strerror(errno));
		return 0;
	}
		
	pd.fd = e->fd;
	pd.events = POLLIN;
	t = time(0);
	for (;;) {
		nt = time(0);
		if (nt-t >= waitsecs)
			return 0;
		if (poll(&pd, 1, waitsecs*1000) > 0)
		if ((n = read(e->fd, e->pkt, sizeof e->pkt)) > 0)
		if (jinput(e))
			t = time(0);
	}
}

/* return 1 == useful, 0 == not useful */
int
jinput(Eth *e)
{
	Aoe *a;
	Qc *q;
	Ata *aa;
	Lun *lun;
	ulong tag;
	int n;

	a = (Aoe *) e->pkt;
	if ((a->flags & (Arsp|Aerr)) != Arsp)
		return 0;
	if ((a->tag[0] & 0x80) == 0)
		return 0;
	tag = nhgetl(a->tag);
	switch (a->cmd) {
	case Cqc:
		q = (Qc *) a;
		lun = findlun(e, a);
		if (lun->state == Lnew) {
			lun->nsect = q->scnt;
			jprobe(e, lun);
			lun->state = Lprobe;
			break;
		}
		return 0;
	case Cata:
		aa = (Ata *) a;
		lun = findlun(e, a);
		if (lun == nil)
			return 0;
		if (lun->id != tag>>16) {
			printf("lun->id %d != tag %ld for %d.%d\n", lun->id, tag>>16, lun->major, lun->minor);
			return 0;
		}
		n = tag & 0xff;
		if (n > lun->maxsect)
			lun->maxsect = n;
		break;
	default:
		return 0;
	}
	return 1;
}

void
hnputl(uchar *p, ulong n)
{
	*p++ = n >> 24;
	*p++ = n >> 16;
	*p++ = n >> 8;
	*p = n;
}

void
hnputs(uchar *p, ushort s)
{
	*p++ = s >> 8;
	*p = s;
}

ushort
nhgets(uchar *p)
{
	ushort s;

	s = *p++;
	s <<= 8;
	s += *p++ & 0xff;
	return s;
}

ulong
nhgetl(uchar *p)
{
	ulong n;

	n = *p++;
	n <<= 8;
	n += *p++ & 0xff;
	n <<= 8;
	n += *p++ & 0xff;
	n <<= 8;
	n += *p++ & 0xff;
	return n;
}

void
jprobe(Eth *e, Lun *lun)
{
	Aoe *a;
	Ata *aa;
	int n;

	memset(e->pkt, 0, sizeof e->pkt);
	a = (Aoe *) e->pkt;
	aa = (Ata *) a;
	memcpy(a->dst, lun->ea, 6);
	memcpy(a->src, e->ea, 6);
	hnputs(a->type, ETaoe);
	hnputs(a->major, lun->major);
	a->minor = lun->minor;
	a->flags = 0x10;
	hnputl(a->tag, lun->id<<16);
	aa->cmd = 0xec;
	aa->scnt = 1;
	n = e->mtu - sizeof *aa;
	for (n &= ~511; n > 0; n -= 512) {
		a->tag[3] = n/512;
		if (write(e->fd, a, sizeof *aa + n) <= 0) {
			printf("write failed\n");
		}
		usleep(100);
	}
}

Lun *
findlun(Eth *e, Aoe *a)
{
	Lun *p, **pp;
	int maj, n;
	static int id;

	maj = nhgets(a->major);
	pp = &e->luns;
	for (; (p=*pp); pp=&p->next) {
		if (maj < p->major)
			continue;
		if (maj > p->major)
			break;
		if (a->minor < p->minor)
			continue;
		if (a->minor > p->minor)
			break;
		n = memcmp(p->ea, a->src, 6);
		if (n < 0)
			continue;
		if (n > 0)
			break;
		return p;
	}
	if (a->cmd == Cata)
		return nil;
	p = mallocz(sizeof *p);
	p->major = maj;
	p->minor = a->minor;
	memmove(p->ea, a->src, 6);
	p->next = *pp;
	p->id = 0x8000 | id++;
	inserttarg(p->major, p->minor);
	return *pp = p;
}	
	

void
catch(int sig)
{
}

int
getindx(int sfd, char *name)  // return the index of device 'name'
{
	struct ifreq xx;
	int n;

    strcpy(xx.ifr_name, name);
    n = ioctl(sfd, SIOCGIFINDEX, &xx);
    if (n == -1)
        return -1;
    return xx.ifr_ifindex;
}

int
getmtu(Eth *e)
{
    struct ifreq xx;
    int n;

    strcpy(xx.ifr_name, e->name);
    n = ioctl(e->fd, SIOCGIFMTU, &xx);
    if (n == -1) {
        perror("Can't get mtu");
        return 1500;
    }
    return xx.ifr_mtu;
}

int
ethopen(Eth *e)	  // get us a raw connection to an interface
{   
	int n, sfd, rbsz, sbsz;
	struct sockaddr_ll sa;
	struct ifreq xx;
	
	rbsz = 64*1024*1024;
	sbsz = 64*1024*1024; 
	memset(&sa, 0, sizeof sa);
	memset(&xx, 0, sizeof xx);
	sfd = socket(PF_PACKET, SOCK_RAW, htons(ETaoe));
	if (sfd == -1) {
		perror("got bad socket");
		return -1;
	}
	if (setsockopt(sfd, SOL_SOCKET, SO_RCVBUFFORCE, &rbsz, sizeof rbsz) < 0)
		fprintf(stderr, "Failed to set socket rcvbuf size\n");
	if (setsockopt(sfd, SOL_SOCKET, SO_SNDBUFFORCE, &sbsz, sizeof sbsz) < 0)
		fprintf(stderr, "Failed to set socket sndbuf size\n");
	n = getindx(sfd, e->name);
	sa.sll_family = AF_PACKET;
	sa.sll_protocol = htons(ETaoe);
	sa.sll_ifindex = n;
	n = bind(sfd, (struct sockaddr *)&sa, sizeof sa);
	if (n == -1) {
		perror("bind funky");
		return -1;
	}
	strcpy(xx.ifr_name, e->name);
	n = ioctl(sfd, SIOCGIFHWADDR, &xx);
	if (n == -1) { 
		perror("Can't get hw addr");
		return -1;
	}
	memmove(e->ea, xx.ifr_hwaddr.sa_data, 6);
	e->fd = sfd;
	e->mtu = getmtu(e);
	return 0;
}

int
ethlist(char **ifs, int nifs)
{
	int i, s, n;
	struct ifreq ifr;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return 0;
	n = 0;
	for (i=0; i<nifs; i++) {
		memset(&ifr, 0, sizeof ifr);
		ifr.ifr_ifindex = i;
		if (ioctl(s, SIOCGIFNAME, &ifr) < 0)
			continue;
		if (strncmp(ifr.ifr_name, "eth", 3))
			continue;
		inserteth(ifs, nifs, ifr.ifr_name);
		n++;
	}
	close(s);
	return n;
}

void *
mallocz(int sz)
{
	void *p;

	p = malloc(sz);
	memset(p, 0, sz);
	return p;
}

void
inserttarg(int maj, int min)
{
	Targ *nt, *t, *p;

	nt = mallocz(sizeof *nt);
	nt->major = maj;
	nt->minor = min;
	if (targlist == NULL) {
		targlist = nt;
		return;
	}
	if (nt->major < targlist->major) { 
		nt->next = targlist;
		targlist = nt;
		return;
	} else if (nt->major == targlist->major && nt->minor < targlist->minor) {
		nt->next = targlist;
		targlist = nt;
		return;
	} else if (nt->major == targlist->major && nt->minor == targlist->minor) 
		return;
	for (p = targlist,t = targlist->next; t; p = t,t = t->next) {
		if (nt->major == t->major && nt->minor == t->minor)
			return;
		if (nt->major < t->major) { 
			p->next = nt;
			nt->next = t;
			return;
		} else if (nt->major == t->major && nt->minor < t->minor) {
				p->next = nt;
				nt->next = t;
				return;
		}
	}
	p->next = nt;
}

void
printsancheck()
{
	Targ *t;

	printf("==========================================\n");
	printf("INTERFACE SUMMARY\n");
	printf("==========================================\n");
	printf("Name\tStatus\tMTU\tPCI ID\n");
	ifsummary();
	printf("==========================================\n");
	printf("DEVICE SUMMARY\n");
	printf("==========================================\n");
	printf("Device\tMacs\tPayload\tLocal Interfaces\n");
	for (t = targlist; t; t = t->next) {
		sancheck(t->major, t->minor);
	}
}

void
ifsummary()
{
	Eth *e;
	char buf[32];
	char *p;

	for (e=eth; e->name; e++) { 
		p = getpciid(buf, sizeof buf, e->name);
		printf("%s\t%s\t%d\t%s\n", e->name, (e->up ? "UP" : "DN"), e->mtu, (p ? p : ""));
	}

}

void
sancheck(int maj, int min)
{
	Eth *e;
	Lun *l;
	Mac *ml, *m;
	int a, found;
	int ps, nsect, nea, nloc;
	int mtu, mtuflag;
	char buf[128];
	char mac[13];

	nsect = mtu = 0;
	mtuflag = 0;
	nloc = nea = 0;
	a = 0;
	ps = 0;
	memset(buf, 0, sizeof buf);
	ml = NULL;

	for (e=eth; e->name; e++) {
		found = 0;
		for (l = e->luns; l; l = l->next) {
			if (!(l->major == maj && l->minor == min))
				continue;
			found = 1;
			if (mtu == 0)
				mtu = e->mtu;
			else if (mtu != e->mtu) {
				mtuflag = 1;
				mtu = (e->mtu > mtu ? e->mtu : mtu);
			}
			insertmac(&ml, l->ea);
			if (ps == 0)
				ps = l->maxsect;
			else
				ps = (l->maxsect < ps ? l->maxsect : ps);
			nsect = l->nsect;
		}
		if (found) {
			snprintf(buf + a,(sizeof buf) - a, "%s%s", (a ? ",": ""), e->name);
			a += strlen(e->name) + (a ? 1 : 0);
			nloc++;
		}
	}
	for (m = ml; m; ml = m, nea++) {
		m = m->next;
		free(ml);
	}
	printf("e%d.%d\t%4d\t%d\t%s\n", maj, min, nea, nsect*512, buf); 
	if (nea != nloc)
		printf("   Mismatched number of local vs remote interfaces\n");
	for (e=eth; e->name; e++) {
		found = 0;
		for (l = e->luns; l; l = l->next)
			if (l->major == maj && l->minor == min) {
				found = 1;
				if (l->maxsect < (e->mtu-32)/512)
					printf("   The path %s->%s is only capable of %d byte payloads\n", e->name, cea(mac, l->ea), l->maxsect*512);
			}
		if (found) {
			if (e->mtu < nsect*512 + 36)
				printf("   %s: MTU (%d) not set optimally for device's capable payload\n",e->name, e->mtu);
			if (e->mtu < mtu)
				printf("   %s: MTU different from other interfaces (%d vs. %d)\n", e->name, e->mtu, mtu);
		}
	}
}

void
insertmac(Mac **ml, char *m)
{
	Mac *nm,*p, *pp;

	for (p = *ml, pp=NULL; p; pp = p, p = p->next)
		if (memcmp(p->ea, m, 6) == 0)
			return;
	nm = mallocz(sizeof *nm);
	memcpy(nm->ea, m, 6);
	if (pp)
		pp->next = nm;
	else
		*ml = nm;	
}

int
ifup(char *ethname)
{
	struct ifreq ifr;
	int r, s;

	memset(&ifr, 0, sizeof ifr);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return 0;
	strcpy(ifr.ifr_name, ethname);
	if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
		close(s);
		return 0;
	}
	r = ifr.ifr_flags & IFF_UP;
	close(s);
	return r;
}

void
inserteth(char **ifs, int nifs, char *n)
{
	int i, j;
	char a[64], b[64];
	
	memset(a, 0, sizeof a);
	memset(b, 0, sizeof b);
	for (i=0; i < nifs; i++) {
		if (ifs[i] == 0) { 
			ifs[i] = strdup(n);
			break;
		}
		j = strcmp(n, ifs[i]);
		if (j < 0) {
			strcpy(a, n);
			for (; ifs[i]; i++) {
				strcpy(b, ifs[i]);
				free(ifs[i]);
				ifs[i] = strdup(a); 
				strcpy(a, b);
			}
			ifs[i] = strdup(a);
			break;
		} else if (j == 0)
			break;
		else if (j > 0) 
			continue;
	}
}

char *
getpciid(char *b, int blen, char *n)
{
	FILE *fd;
	char dev[8];
	char ven[8];
	char path[128];

	memset(dev, 0, sizeof dev);
	memset(ven, 0, sizeof ven);
	memset(path, 0, sizeof path);
	sprintf(path, "/sys/class/net/%s/device/vendor", n);
	if ((fd = fopen(path, "r")) == NULL)
		return NULL;
	fseek(fd, 2, SEEK_SET);
	if (fread(ven, 1, 4, fd) <= 0) {
		fclose(fd);
		return NULL;
	}
	fclose(fd);
	        sprintf(path, "/sys/class/net/%s/device/device", n);
        if ((fd = fopen(path, "r")) == NULL)
                return NULL;
        fseek(fd, 2, SEEK_SET);
        if (fread(dev, 1, 4, fd) <= 0) {
                fclose(fd);
                return NULL;
        }
        fclose(fd);
	snprintf(b, blen, "%s:%s", ven, dev);	
	return b;
}

