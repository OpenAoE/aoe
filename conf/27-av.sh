echo new; exit			# disable this test

f=linux/drivers/block/aoe/aoecmd.c
old="`fgrep 'static const char *empty_page;' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
