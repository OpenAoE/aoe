new="`grep '[-]>i_mutex' linux/drivers/block/aoe/aoecmd.c`"
if test "$new"; then
	echo new
else
	echo old
fi
