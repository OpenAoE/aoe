#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <scsi/sg.h>

static ssize_t aoedisk_show_state(struct device *dev,
				  struct device_attribute *attr, char *page)
{
	struct kobject *kobj = &dev->kobj; /* dev should have kobj */
	struct gendisk *gd = NULL;

	return kobj - (struct kobject *) &gd->dev; /* gd should have dev */
}
static DEVICE_ATTR(state, S_IRUGO, aoedisk_show_state, NULL);
static struct attribute *aoe_attrs[] = {
	&dev_attr_state.attr,
	NULL,
};
