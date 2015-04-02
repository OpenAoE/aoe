#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>

static int
test_12_fn(struct kobject *kobj, const struct attribute_group *agrp)
{
	return sysfs_create_group(kobj, agrp);
}
