#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/netdevice.h>
#include <linux/moduleparam.h>

int testfn(struct sk_buffer *skb)
{
	return skb_linearize(skb, GFP_ATOMIC);
}
