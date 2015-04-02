#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include <scsi/sg.h>

int test_fn(struct dentry *e)
{
	return IS_ERR_OR_NULL(e);
}
