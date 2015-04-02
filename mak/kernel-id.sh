#! /bin/sh

d="${1?}"
{
    echo $d
    echo .config: `sh mak/checksum.sh $d/.config`
} > mak/kernel-id.out
sh mak/update-if-new.sh kernel-id
