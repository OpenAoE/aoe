. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep -i '(warning|error).*sysfs_create_group' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	echo old
else
	echo new
fi
