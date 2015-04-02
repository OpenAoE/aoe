. conf/kv.sh
echo 'obj-$(CONFIG_ATA_OVER_ETH) += '"$TESTNO"'.o' > conf/Makefile
$make_cmd > conf/$TESTNO.log 2>&1

status="$?"
test "`egrep -i '(err|warn).*compound_trans_head' conf/$TESTNO.log`" && { echo old; exit 0; }
test "$status" != "0" && {
	echo `basename $0` Error: cannot identify target kernel state for compatibility 1>&2
	echo unsupported
	exit 1
}

echo new
