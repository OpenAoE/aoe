f=linux/drivers/block/aoe/aoeblk.c
new="`fgrep 'aoeblk_getgeo(struct block_device *bdev, struct hd_geometry *geo)' $f`"
if test "$new"; then
	echo new
else
	echo old
fi
