f=linux/drivers/block/aoe/aoechr.c
old="`fgrep 'unsigned long msleep_interruptible(unsigned int msecs)' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
