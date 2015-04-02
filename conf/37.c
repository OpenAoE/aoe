#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>

void *testfn(void *p, int n)
{
	return kmemdup(p, n, GFP_KERNEL);
}
