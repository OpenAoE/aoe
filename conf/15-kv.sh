. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep -i 'implicit declaration.*skb_mac_header' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo old
else
	echo new
fi
