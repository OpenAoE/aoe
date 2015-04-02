f=linux/drivers/block/aoe/aoeblk.c

bpr="`fgrep 'blk_peek_request(' $f`"
test "$bpr" && { echo new; exit 0; }

echo old
