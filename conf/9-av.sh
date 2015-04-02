f=linux/drivers/block/aoe/aoeblk.c
old="`fgrep 'atomic_dec(&bv->bv_page->count);' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
