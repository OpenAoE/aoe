#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <linux/sysfs.h>
#include <scsi/sg.h>

struct attribute aoe_attrs = {
	.owner = NULL,
};
void *testfn(struct attribute *p)
{
	return (void *) p->owner;
}
