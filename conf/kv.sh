# common code for *-kv.sh

TESTNO=`echo $0 | sed 's!^conf/!!' | sed 's!-kv[.]sh$!!'`
echo 'obj-$(CONFIG_ATA_OVER_ETH)	+= '"$TESTNO"'.o' > conf/Makefile
