#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/genhd.h>
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
/* #include <net/net_namespace.h> */
#include <asm/unaligned.h>
#include <linux/uio.h>

unsigned short
rqbiocnt(struct request *r)
{
	struct bio *bio;
	unsigned short n = 0;

	rq_for_each_bio(bio, r)
		n++;
	return n;
}
