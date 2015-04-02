#include <linux/ata.h>
#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/genhd.h>
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <net/net_namespace.h>
#include <asm/unaligned.h>
#include <linux/uio.h>

struct page *testfn(struct page *page)
{
	return compound_trans_head(page);
}
