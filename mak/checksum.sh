#! /bin/sh
# Use whatever they have to checksum the file.

f="${1:-/dev/stdin}"

test -r "$f" || {
	exec 1>&2
	echo "`basename $0` Error: \"$f\" is not readable"
	exit 1
}
{
	md5sum "$f" 2> /dev/null ||
	sha1sum "$f" 2> /dev/null ||
	cksum "$f" 2> /dev/null ||
	sum "$f"
} | awk '{print $1}'
