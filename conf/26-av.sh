f=linux/drivers/block/aoe/aoecmd.c
old="`fgrep 'static inline void list_replace(struct list_head *old,' $f`"
if test "$old"; then
	echo old
else
	echo new
fi
