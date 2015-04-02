# This script is run from its parent directory.

revert=
if test "$1" = "-R"; then
	revert=yes
	shift
fi

driver_d=$1
shift
make_cmd="$*"

export driver_d make_cmd

me=`basename $0`

# ignore errors when reverting configuration
if test "$revert" != "yes"; then
	set -e
fi

# the compatibility tests recognize compiler messages, so turn
# off internationalization during the tests
LANG=C
LC_ALL=C
# openSUSE 11.4 doesn't find LC_ALL convincing, requiring LC_CTYPE
LC_CTYPE=C
export LANG LC_ALL LC_CTYPE

logf=conf/compat.log
: > $logf

cleanup() {
	i=$1
	rm -f conf/$i.o conf/$i.ko conf/$i.mod.[co] conf/.$i.*.cmd conf/.$i.*.d
	rm -f conf/$i-*.o conf/$i-*.ko \
		conf/$i-*.mod.[co] conf/.$i-*.*.cmd conf/.$i-*.*.d
}

i=1
while test -r conf/$i-kv.sh; do
	printf "$i "
	kv=`sh conf/$i-kv.sh`
	av=`sh conf/$i-av.sh`
	echo "$i kv=$kv av=$av" >> $logf

	if test "$revert" = "yes" && test "$av" != "new"; then
		if test -r "conf/$i-patches/old-new.diff"; then
			kv=new
		else
			kv=`awk '$NF=="NEW"{print $1}' conf/$i.txt`
		fi
	fi

	if test "$kv" = "$av"; then
		cleanup $i
		i=`expr $i + 1`
		continue
	fi

	p="conf/$i-patches/$av-$kv.diff"
	reverse=
	test -r "$p" || {
		p="conf/$i-patches/$kv-$av.diff"
		reverse="-R"
	}
	test -r "$p" || {
		echo 1>&2 "$me Error: no $p or inverse found"
		exit 1
	}

	echo
	echo "patch -p1 $reverse < $p" >> $logf
	patch -p1 $reverse < "$p"

	cleanup $i
	i=`expr $i + 1`
done

echo ok
exit 0
