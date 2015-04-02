f=linux/drivers/block/aoe/aoeblk.c

old="`fgrep 'static inline long __must_check IS_ERR_OR_NULL(__force const void *ptr)' $f`"
test "$old" && { echo old; exit 0; }

echo new
