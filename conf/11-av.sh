f=linux/drivers/block/aoe/aoeblk.c
old="`fgrep 'kmem_cache_t' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
