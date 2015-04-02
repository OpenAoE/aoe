f=linux/drivers/block/aoe/aoeblk.c
new="`fgrep 'aoeblk_ioctl(struct block_device *bdev, fmode_t mode, uint cmd, ulong arg)' $f`"
test "$new" && { echo new; exit 0; }

echo old
