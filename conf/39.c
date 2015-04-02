#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>
#include <scsi/sg.h>

int testfn(struct seq_file *s, int n)
{
	return seq_printf(s, "%d\n", n);
}
