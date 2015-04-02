#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/netdevice.h>
#include <linux/moduleparam.h>

static int
aoenet_rcv(struct sk_buff *skb, struct net_device *ifp, struct packet_type *pt)
{
	return 0;
}

static struct packet_type aoe_pt = {
	.type = 0,
	.func = aoenet_rcv,
};
