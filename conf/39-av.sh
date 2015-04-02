f=linux/drivers/block/aoe/aoeblk.c

old="`grep '^#include <linux/seq_file.h>$' $f`"
test "$old" && { echo old; exit 0; }

echo new
