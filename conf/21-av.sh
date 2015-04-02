f=linux/drivers/block/aoe/aoeblk.c
skip="`fgrep 'sysfs_create_file(&d->gd->kobj, &disk_attr_state.attr);' $f`"
test "$skip" && { echo skip; exit 0; }

dtd="`fgrep 'return sysfs_create_group(&disk_to_dev(d->gd)->kobj,' $f`"
test "$dtd" && { echo dtd; exit 0; }

dev="`fgrep 'static ssize_t aoedisk_show_state(struct device *dev,' $f`"
test "$dev" && { echo dev; exit 0; }

echo gd
