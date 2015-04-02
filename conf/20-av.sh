f=linux/drivers/block/aoe/aoecmd.c
new="`fgrep '__rq_for_each_bio(' $f`"
if test "$new"; then
	echo new
else
	echo old
fi
