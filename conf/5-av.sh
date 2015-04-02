new="`grep '^aoenet_rcv.*orig_dev' linux/drivers/block/aoe/aoenet.c`"
if test "$new"; then
	echo new
else
	echo old
fi
