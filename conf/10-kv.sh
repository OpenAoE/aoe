. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if grep 'passed 3 arguments, but takes just 2' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo new 
else
	echo old
fi
