. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if test "$?" = "0" &&
	test ! "`grep 'incompatible pointer' conf/$TESTNO.log`"; then
	echo new
	exit 0
fi

echo old
