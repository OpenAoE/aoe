f=linux/drivers/block/aoe/aoecmd.c
old="`grep 'define for_each_netdev(__ifp)' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
