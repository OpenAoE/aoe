#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>

void foo(void *);

void
testfn_10(struct work_struct *w)
{
	INIT_WORK(w, foo, NULL);
}

