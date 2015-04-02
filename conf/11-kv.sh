. conf/kv.sh
$make_cmd > conf/$TESTNO.log 2>&1
result=old
if egrep -i '(warning|error).*kmem_cache_t' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	result=new 
fi
# gcc-3.4.4 on debian says "parse error", but 3.4.6 corrects that
if egrep -i '(parse|syntax) error before' conf/$TESTNO.log \
	> /dev/null 2>&1; then
	result=new 
fi
if egrep -i "expected.*before '[*]' token" conf/$TESTNO.log \
	> /dev/null 2>&1; then
	result=new
fi
echo $result
