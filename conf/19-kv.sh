. conf/kv.sh
echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-dev_net"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-dev_net.log 2>&1
test "$?" = "0" && { echo dev_net; exit 0; }

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-nd_net"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-nd_net.log 2>&1
test "$?" = "0" && { echo nd_net; exit 0; }

echo nons
