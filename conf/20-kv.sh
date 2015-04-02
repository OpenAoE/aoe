. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep 'declar.*rq_for_each_bio' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo new
else
	echo old
fi
