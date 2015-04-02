#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>

void *
testing_function(char *name)
{
	void *p;

        p = kmem_cache_create(name,
		100,
		0, 0, NULL, NULL);

	return p;
}
