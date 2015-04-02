start=${1?}
end=`cd conf && ls *.diff | sort -n | tail -n 1`
end=`basename $end .diff`
for i in `seq $start $end | tac`; do
	n=`expr $i + 1`
	for s in -av.sh -kv.sh .c .diff .txt; do
		echo git mv conf/$i$s conf/$n$s
	done
done
