f=linux/drivers/block/aoe/aoeblk.c
new="`sed -n '/kmem_cache_create/,$p' $f | head | fgrep '0, 0, NULL);'`"
if test "$new"; then
	echo new
else
	echo old
fi
