#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/genhd.h>
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <asm/unaligned.h>
#include <linux/uio.h>

void
testfn_brq(struct request_queue *q)
{
	__blk_run_queue(q, 0);
}
