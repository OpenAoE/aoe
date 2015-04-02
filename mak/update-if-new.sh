#! /bin/sh

s="${1?}"

if test -r mak/${s}.last; then
	cmp mak/${s}.out mak/${s}.last > mak/${s}.cmpout 2>&1 ||
		echo > mak/${s}.ts
else
	echo > mak/${s}.ts
fi
cp mak/${s}.out mak/${s}.last
