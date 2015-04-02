f=linux/drivers/block/aoe/aoechr.c
# skip this compat test if 2 made the aoe sources use class_simple
old="`grep 'class_simple' $f`"
test "$old" && { echo skip; exit 0; }

# look to see whether the source is already new-style
cdc5="`grep 'class_device_create.aoe_class, NULL' $f`"
test "$cdc5" && { echo cdc5; exit 0; }

cdc4="`grep 'class_device_create.aoe_class,$' $f`"
test "$cdc4" && { echo cdc4; exit 0; }

dcd="`grep 'device_create_drvdata.aoe_class, NULL' $f`"
test "$dcd" && { echo dcd; exit 0; }

dc="`fgrep 'device_create(aoe_class, NULL,' $f`"
test "$dc" && { echo dc; exit 0; }

echo "`basename $0` Error: could not identify aoe source state" 1>&2
exit 1
