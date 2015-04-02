. conf/kv.sh

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-brq1"'.o' > conf/Makefile
log=conf/$TESTNO-brq1.log
$make_cmd > $log 2>&1
test "$?" = "0" &&
	test -z "`egrep -i '(warning|error).*blk_run_queue' $log`" &&
	{ echo brq1; exit 0; }

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-brq2"'.o' > conf/Makefile
log=conf/$TESTNO-brq2.log
$make_cmd > $log 2>&1
test "$?" = "0" &&
	test -z "`egrep -i '(warning|error).*blk_run_queue' $log`" &&
	{ echo brq2; exit 0; }

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-bsq"'.o' > conf/Makefile
log=conf/$TESTNO-bsq.log
$make_cmd > $log 2>&1
test "$?" = "0" &&
	test -z "`egrep -i '(warning|error).*blk_start_queueing' $log`" &&
	{ echo bsq; exit 0; }

echo old
