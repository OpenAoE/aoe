f=linux/drivers/block/aoe/aoecmd.c
new="`fgrep 'end_that_request_last(rq);' $f`"
if test "$new"; then
	echo old
else
	echo new
fi
