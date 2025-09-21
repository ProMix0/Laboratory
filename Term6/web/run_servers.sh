#!/bin/sh

set -e

for dir in lab{2,3,4,5,6,7,8} rgr; do
	echo Starting $dir
	cd $dir && go run . &
done

wait $(jobs -p)
