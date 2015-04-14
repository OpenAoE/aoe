f=linux/drivers/block/aoe/aoeblk.c

old="`sed -n '/^static int/{n;/^aoeblk_release/p;}' $f`"
test "$old" && { echo old; exit 0; }

echo new
