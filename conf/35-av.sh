f=linux/drivers/block/aoe/aoechr.c

old="`fgrep 'lock_kernel()' $f`"
test "$old" && { echo old; exit 0; }

echo new
