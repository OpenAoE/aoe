#! /bin/sh

d="${1?}"
for f in "$d"/*.[ch]; do
	echo "$f:" `sh mak/checksum.sh "$f"`
done > mak/src-id.out
sh mak/update-if-new.sh src-id

