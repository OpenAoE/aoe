f=linux/drivers/block/aoe/aoecmd.c

old="`fgrep 'skb_fillup(struct sk_buff *skb, struct bio_vec *bv, ulong off, ulong cnt)' $f`"
test "$old" && { echo old; exit 0; }

echo new
