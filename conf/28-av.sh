f=linux/drivers/block/aoe/aoeblk.c
new="`fgrep 'aoeblk_open(struct block_device *bdev, fmode_t mode)' $f`"
if test "$new"; then
	echo new
else
	echo old
fi
