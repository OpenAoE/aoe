#include <linux/skbuff.h>
#include <linux/hdreg.h>
#include <linux/mempool.h>
#include <linux/debugfs.h>
#include <linux/relay.h>

static struct dentry *
cfile(const char *filename,
      struct dentry *parent,
      umode_t mode,
      struct rchan_buf *buf,
      int *is_global)
{
	return NULL;
}

struct rchan_callbacks cb = {
	.create_buf_file = cfile,
};
