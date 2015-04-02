#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>

void test_function(struct page *p)
{
	atomic_inc(p->_count);
	atomic_dec(p->_count);	
}
