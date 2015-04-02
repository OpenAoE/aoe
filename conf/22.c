#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <scsi/sg.h>

static int
aoeblk_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	geo->cylinders = 0;
	geo->heads = 0;
	geo->sectors = 0;
	return 0;
}

void
testfn_21(struct block_device_operations *bdops)
{
	bdops->getgeo = aoeblk_getgeo;
};
