f=linux/drivers/block/aoe/aoedbg.c

old="`sed -n '/^cfile/{n;n;/^int mode,/p;}' $f`"
test "$old" && { echo old; exit 0; }

echo new
