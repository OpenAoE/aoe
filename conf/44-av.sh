f=linux/drivers/block/aoe/aoecmd.c

old="`fgrep 'bok = !fastfail && test_bit(BIO_UPTODATE, &bio->bi_flags);' $f`"
test "$old" && { echo old; exit 0; }

echo new
