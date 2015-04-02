. conf/kv.sh
if $make_cmd > conf/$TESTNO.log 2>&1; then
	echo new
else
	echo old
fi
