. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if grep 'has no member named .*_count' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo old
else
	echo new
fi
