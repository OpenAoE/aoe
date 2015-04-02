#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/init.h>

struct aoe_chardev {
	ulong minor;
	char name[32];
};

static struct class *aoe_class;
static struct aoe_chardev chardevs[1];

int
aoechr_init(void)
{
	class_device_create(aoe_class, NULL,
			    MKDEV(152, chardevs[0].minor),
			    NULL, chardevs[0].name);

	return 0;
}
