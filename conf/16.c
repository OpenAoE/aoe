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

int *test_fn(struct net_device *ifp)
{
	int n = 0;
	
	read_lock(&dev_base_lock);
	for_each_netdev(&init_net, ifp) {
		dev_hold(ifp);
		n += 1;
		dev_put(ifp);
	}
	read_unlock(&dev_base_lock);
}
