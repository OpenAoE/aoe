#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <scsi/sg.h>

static int
aoeblk_open(struct block_device *bdev, fmode_t mode)
{
	char *p = (char *) bdev->bd_disk->private_data;
	char *q = (char *) bdev;

	return p - q;
}

struct block_device_operations aoe_bdops = {
	.open = aoeblk_open,
};
