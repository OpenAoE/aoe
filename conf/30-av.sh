f=linux/drivers/block/aoe/aoecmd.c
new="`fgrep '#include <linux/ata.h>' $f`"
test "$new" && { echo new; exit 0; }

echo old
