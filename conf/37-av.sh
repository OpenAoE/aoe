f=linux/drivers/block/aoe/aoechr.c

old="`fgrep 'static void *kmemdup(const void *src, size_t len, gfp_t gfp)' $f`"
test "$old" && { echo old; exit 0; }

echo new
