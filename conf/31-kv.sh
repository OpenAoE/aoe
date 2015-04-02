. conf/kv.sh

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO"'.o' > conf/Makefile
log=conf/$TESTNO.log
$make_cmd > $log 2>&1
test "$?" = "0" &&
	test -z "`egrep -i '(warning|error).*blk_start_request' $log`" &&
	{ echo new; exit 0; }

echo old
