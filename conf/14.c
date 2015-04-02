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

static void
test_13_fn(struct request *q, int uptodate)
{
	end_that_request_last(q, uptodate);
}
