f=linux/drivers/block/aoe/aoecmd.c

bsq="`fgrep 'blk_start_queueing(' $f`"
test "$bsq" && { echo bsq; exit 0; }

brq1="`fgrep '__blk_run_queue(q);' $f`"
test "$brq1" && { echo brq1; exit 0; }

brq2="`fgrep '__blk_run_queue(q, 0);' $f`"
test "$brq2" && { echo brq2; exit 0; }

echo old
