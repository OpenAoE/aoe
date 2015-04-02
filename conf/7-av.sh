f=linux/drivers/block/aoe/aoecmd.c
old="`grep 'skb_linearize(skb, GFP_ATOMIC' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
