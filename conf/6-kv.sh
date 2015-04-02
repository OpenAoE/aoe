. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if grep '.*struct.* has no member named .i_mutex' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo old
else
	echo new
fi
