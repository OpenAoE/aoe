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
skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac.raw = skb->data;
}

void test_fn(struct sk_buff *skb)
{
	skb_reset_mac_header(skb);
}
