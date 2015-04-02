#! /bin/sh

make_cmd="$*"
$make_cmd V=1 > mak/dummy.log 2>&1
`grep '[-]nostdinc' mak/dummy.log | awk '{print $1}' | sed 1q` --version \
	> mak/cc-id.out
rm -f mak/dummy.o mak/dummy.ko
sh mak/update-if-new.sh cc-id
