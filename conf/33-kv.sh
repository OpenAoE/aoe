. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep -i '(implicit declaration).*blk_queue_max_sectors' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo new
else
	echo old
fi
