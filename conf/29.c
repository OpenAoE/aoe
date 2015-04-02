#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <scsi/sg.h>

static int
test_ioctl(struct inode *inode, struct file *filp, uint cmd, ulong arg)
{
	struct inode *p = (struct inode *) filp;

	return p - inode;
}

void
testfn(struct block_device_operations *bdops)
{
	bdops->ioctl = test_ioctl;
}
