f=linux/drivers/block/aoe/aoeblk.c
new="`fgrep 'sysfs_create_group' $f`"
if test "$new"; then
	echo new
else
	echo old
fi
