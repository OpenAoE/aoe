#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/netdevice.h>
#include <linux/delay.h>
#include <linux/bitmap.h>
#include <linux/kdev_t.h>

static DEFINE_SPINLOCK(test_lock);

void *testfn(int n)
{
	return n > 0 ? &test_lock : NULL;
}
