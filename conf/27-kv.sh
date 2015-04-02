echo new; exit			# disable this test

. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
if egrep \
	"empty_zero_page.*undefined" \
	conf/$TESTNO.log > /dev/null 2>&1; then
	echo old
elif egrep \
	"phys_base.*undefined" \
	conf/$TESTNO.log > /dev/null 2>&1; then
	echo old
else
	echo new
fi
