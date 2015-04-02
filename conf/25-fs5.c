#include <linux/relayfs_fs.h>

void
test_fn(char *name)
{
	relay_open(name, NULL, 1024*128, 4, NULL);
}
