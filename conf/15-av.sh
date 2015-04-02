f=linux/drivers/block/aoe/aoecmd.c
old="`grep 'skb_mac_header(struct sk_buff \*skb)$' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
