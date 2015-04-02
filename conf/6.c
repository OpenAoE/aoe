#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/genhd.h>
#include <asm/unaligned.h>

void testfn(struct block_device *bd)
{
	mutex_lock(&bd->bd_inode->i_mutex);
	mutex_unlock(&bd->bd_inode->i_mutex);
}
