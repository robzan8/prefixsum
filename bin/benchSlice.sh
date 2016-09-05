#!/bin/bash

# $1 is maximum number of workers, $2 is array length, $3 is active wait cycles
for i in `seq 1 $1`; do
	for j in {1..3}; do
		TIME[$j]=`./testSlice $i $2 $3`
	done
	SORTED_STR=`echo ${TIME[*]} | tr " " "\n" | sort -g | tr "\n" " "`
	SORTED_ARR=($SORTED_STR)
	echo ${SORTED_ARR[2]}
done
