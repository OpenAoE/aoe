/* If this file won't compile, then struct disk_attribute is likely
 * defined in the kernel headers, i.e., 2.6.11 or later.
 */
#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/netdevice.h>

struct disk_attribute da_test;
