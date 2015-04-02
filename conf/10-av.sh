f=linux/drivers/block/aoe/aoedev.c
old="`grep 'INIT_WORK(.*,.*,.*)' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
