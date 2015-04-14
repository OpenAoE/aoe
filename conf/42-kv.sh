. conf/kv.sh
echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO.log 2>&1

status="$?"
test "`egrep -i 'release' conf/$TESTNO.log`" && { echo new; exit 0; }

echo old
