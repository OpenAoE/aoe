f=linux/drivers/block/aoe/aoeblk.c

# 34 becomes not applicable when 21 moved the sources to the gd state
# (It's conf/21 in Nov 2010, but it could get renumbered someday.)
gd="`fgrep 'static ssize_t aoedisk_show_netif(struct gendisk *disk, char *page)' $f`"
test "$gd" && { echo skip; exit 0; }

owner="`sed -n '/\.attr = {/{p;n;p;n;p;n;p;n;p;}' $f | fgrep '.owner = THIS_MODULE,'`"
test "$owner" && { echo old; exit 0; }

echo new
