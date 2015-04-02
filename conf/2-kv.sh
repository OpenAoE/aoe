. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1

# RedHat's gcc puts weird chars in messages, so we need .* below
if grep ': implicit declaration of function .*class_simple_create' \
	conf/$TESTNO.log > /dev/null 2>&1; then
	echo new
else
	echo old
fi
