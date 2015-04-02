. conf/kv.sh
echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO.log 2>&1
if test "$?" = "0" &&
	test ! "`grep 'incompatible pointer' conf/$TESTNO.log`"; then
	echo old
	exit 0
fi

echo new

