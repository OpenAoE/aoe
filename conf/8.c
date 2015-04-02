#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/init.h>
#include <linux/delay.h>

void test_function(void)
{
	msleep_interruptible(200);
}
