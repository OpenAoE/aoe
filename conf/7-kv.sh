. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if grep 'too many arguments to function.*skb_linearize' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo new
else
	echo old
fi
