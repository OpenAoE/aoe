f=linux/drivers/block/aoe/aoedbg.c

fs="`fgrep '#include <linux/relayfs_fs.h>' $f`"
test "$fs" && { echo fs5; exit 0; }

none="`fgrep '#include <linux/jiffies.h>' $f`"
test "$none" && { echo none; exit 0; }

relay6="`fgrep 'relay_open(base_fnam, parent, subbuf_size, n_subbufs, &cb, NULL);' $f`"
test "$relay6" && { echo relay6; exit 0; }

relay5="`fgrep 'relay_open(base_fnam, parent, subbuf_size, n_subbufs, &cb);' $f`"
test "$relay5" && { echo relay5; exit 0; }

echo "`basename $0` Error: could not identify aoe source state" 1>&2
exit 1
