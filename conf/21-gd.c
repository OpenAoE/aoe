#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <scsi/sg.h>
#include "../linux/drivers/block/aoe/disk_attr.h"

static ssize_t aoedisk_show_state(struct gendisk *gd, char *page)
{
	return &gd->kobj - (struct kobject *) NULL;
}
static struct disk_attribute disk_attr_state = {
	.attr = {.name = "state", .mode = S_IRUGO },
	.show = aoedisk_show_state
};
struct attribute *aoe_attrs[] = {
	&disk_attr_state.attr,
	NULL,
};
