patched=`grep '^skb_mac_header' linux/drivers/block/aoe/aoecmd.c`
test "$patched" = "" && {
	echo normal
	exit 0
}
f=linux/drivers/block/aoe/aoecmd.c
normal="`grep '^skb_reset_mac_header(' $f`"
if test "$normal"; then
	echo normal
else
	echo sles10
fi
