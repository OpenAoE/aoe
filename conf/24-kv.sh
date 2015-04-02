# skip this compat test if 2 made the aoe sources use class_simple
old="`grep 'class_simple' linux/drivers/block/aoe/aoechr.c`"
test "$old" && { echo skip; exit 0; }

. conf/kv.sh
echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-cdc4"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-cdc4.log 2>&1
test "$?" = "0" && { echo cdc4; exit 0; }

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-cdc5"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-cdc5.log 2>&1
test "$?" = "0" && { echo cdc5; exit 0; }

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-dcd"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-dcd.log 2>&1
test "$?" = "0" && { echo dcd; exit 0; }

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-dc"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-dc.log 2>&1
test "$?" = "0" && { echo dc; exit 0; }

echo "`basename $0` Error: could not identify kernel source state" 1>&2
exit 1
