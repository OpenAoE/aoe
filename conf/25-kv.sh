. conf/kv.sh
for t in fs5 relay5 relay6; do
	log=conf/$TESTNO-$t.log
	echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-$t"'.o' > conf/Makefile
	$make_cmd > $log 2>&1
	test "$?" = "0" &&
	test -z "`grep 'relay_open.*undefined' $log`" &&
	test -z "`grep -i 'Module.symvers' $log`" &&
	{ echo $t; exit 0; }
done

echo none
