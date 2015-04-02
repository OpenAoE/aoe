. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep -i 'too many' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo new
else
	echo old
fi
