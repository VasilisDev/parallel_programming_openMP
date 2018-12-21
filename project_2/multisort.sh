
#!/bin/sh

export PROG=multisort
make $PROG

export OMP_NUM_THREADS=8

export size=4
export sort_size=4
export merge_size=4

./$PROG $size $sort_size $merge_size
