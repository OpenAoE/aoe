f=linux/drivers/block/aoe/aoeblk.c
skip="`fgrep 'sysfs_create_file(&d->gd->kobj, &disk_attr_state.attr);' $f`"
if test "$skip"; then
	echo skip
	exit
fi

. conf/kv.sh
echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-gd"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-gd.log 2>&1
if test "$?" = "0" &&
	test ! "`grep 'incompatible pointer' conf/$TESTNO-gd.log`"; then
	echo gd
	exit 0
fi

echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO-dev"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO-dev.log 2>&1
test "$?" = "0" && { echo dev; exit 0; }

echo dtd

