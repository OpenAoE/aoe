#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/init.h>

void *
test_fn(void)
{
	void *p = class_simple_create(THIS_MODULE, "testing");

	return p;
}
