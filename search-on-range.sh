#!/bin/bash

if [ $# -ne 3 ]
  then
    echo "usage: ./search-on-range.sh <max s> <k start> <k end>"
    exit 1
fi

for ((i = $2; i <= $3; i++))
do
  filename="test_results/search_test_$i.out"
  search_heuristic_tester $1 $i 0 1  &> $filename
  echo "-- completed testing -- " >> $filename
done
