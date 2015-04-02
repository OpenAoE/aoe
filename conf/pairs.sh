# Given a set of tags representing kernel states, you don't need to
# have patches that go from one state to the same state, nor do you
# need a patch for each direction, since patch takes "-R".
# 
# If I had had more sleep lately, I probably wouldn't have written
# this script.  As it stands, it will print out all the patches you'd
# need as pairs of tags.
# 
# ecashin@kokone 25$ pwd
# /home/ecashin/aoe-2.6-devel/conf-states/25
# ecashin@kokone 25$ ls
# fs5  none  relay5  relay6
# ecashin@kokone 25$ for i in `ls`; do echo $i; done | sh ~/aoe6-stand/conf/pairs.sh | while read a b; do diff -upr $a $b > ~/aoe6-stand/conf/25-patches/$a-$b.diff; done
# ecashin@kokone 25$ 
# 

awk 'BEGIN{
	i=0
	done["available"] = "yes"
}
{
	a[i] = $0
	b[i] = $0
	i += 1
}
END {
	n = i
	for (i=0; i<n; ++i) {
		aa = a[i]
		for (j=0; j<n; ++j) {
       			bb = b[j]
			s1 = aa ":" bb
			s2 = bb ":" aa
			if (aa != bb && !(s1 in done) && !(s2 in done)) {
				done[s1] = "yes"
				print aa " " bb
			}
		}
	}
}
'
