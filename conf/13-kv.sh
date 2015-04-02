. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep -i 'implicit declaration of function .*__blk_end_request' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo old
else
	echo new
fi
