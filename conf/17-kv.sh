. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep -i 'DEFINE_SPINLOCK' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo old
else
	echo new
fi
