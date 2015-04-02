f=linux/drivers/block/aoe/aoedev.c
old="`grep '#define[ 	][ 	]*DEFINE_SPINLOCK' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
