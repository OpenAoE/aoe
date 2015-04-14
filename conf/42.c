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

static int
release(struct gendisk *disk, fmode_t mode)
{
	return 0;
}

static const struct block_device_operations bdops = {
	.release = release,
};
