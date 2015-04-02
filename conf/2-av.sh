# This script is run from its parent directory.

old="`grep 'class_simple' linux/drivers/block/aoe/aoechr.c`"
if test "$old"; then
	echo old
else
	echo new
fi
