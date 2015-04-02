f=linux/drivers/block/aoe/aoecmd.c
new="`fgrep '__blk_end_request(' $f`"
if test "$new"; then
	echo new
else
	echo old
fi
