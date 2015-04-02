f=linux/drivers/block/aoe/aoeblk.c

# 34 becomes not applicable when 21 moved the sources to the gd state
# (It's conf/21 in Nov 2010, but it could get renumbered someday.)
gd="`fgrep 'static ssize_t aoedisk_show_netif(struct gendisk *disk, char *page)' $f`"
test "$gd" && { echo skip; exit 0; }

. conf/kv.sh
echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO.log 2>&1
if test "$?" = "0" &&
	test ! "`grep 'owner' conf/$TESTNO.log`"; then
	echo old
	exit 0
fi

echo new

