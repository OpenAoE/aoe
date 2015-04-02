f=linux/drivers/block/aoe/aoe.h

old="`fgrep 'static inline struct page *compound_trans_head(struct page *p) {return p;}' $f`"
test "$old" && { echo old; exit 0; }

echo new
