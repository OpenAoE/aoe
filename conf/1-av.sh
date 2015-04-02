old="`grep 'struct attribute attr;' linux/drivers/block/aoe/disk_attr.h`"

if test "$old"; then
	echo old
else
	echo new
fi
