#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <scsi/sg.h>

void test_fn(struct request_queue *q, int n)
{
	blk_queue_max_sectors(q, n);
}
