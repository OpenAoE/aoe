patched=`grep '^skb_mac_header' linux/drivers/block/aoe/aoecmd.c`
test "$patched" = "" && {
	echo normal
	exit 0
}
. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep -i '(previous definition|redefinition).*skb_reset_mac_header' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo sles10
else
	echo normal
fi
