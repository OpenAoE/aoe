f=linux/drivers/block/aoe/aoeblk.c
old="`grep 'blk_queue_max_sectors(' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
