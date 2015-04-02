f=linux/drivers/block/aoe/aoechr.c

new="`fgrep '#include <linux/export.h>' $f`"
test "$new" && { echo new; exit 0; }

echo old
