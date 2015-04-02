#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <scsi/sg.h>

void
testfn(struct request_queue *q)
{
	struct request *rq = blk_peek_request(q);
	blk_start_request(rq);
}
