. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if grep 'initialization from incompatible pointer type' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo new
else
	echo old
fi

