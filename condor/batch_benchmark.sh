#!/bin/bash

rm -f out-util_check*csv
rm -f err-util_check*txt
rm -f benchmark_batch.txt

echo "./batch_benchmark <OnlyCheck|Checkers|Heuristics|All> <start_s> <end_s> <start_k> <end_k> <iter>"
s_min=$(($2))
s_max=$(($3))
k_min=$(($4))
k_max=$(($5))


for ((s=$s_min;s<=$s_max;s++)); do

    for ((k=$k_min;k<=$k_max;k++)); do

	echo $s, $k >> benchmark_batch.txt

    done

done
    

cluster=`condor_submit submit_util_check_benchmark.txt iter=$6 mode=$1 | grep "cluster " | sed -n 's/.*cluster //p' | sed -n 's/\.//p'`
#cluster=0

remaining=`condor_q $cluster | grep "util_check" | wc -l`

while [ $remaining -ne 0 ]
do

    remaining=`condor_q $cluster | grep "util_check" | wc -l`
    echo Processes remaining $remaining "in cluster" $cluster"."
    sleep 1

done

outfile=out-benchmark-$cluster.csv
errfile=err-benchmark-$cluster.txt

cat out-util*csv | head -n 1 > output/$outfile
cat out-util*csv | sed "/USP/d" >> output/$outfile
cat err-util*txt > output/$errfile

errs=`cat output/$errfile | wc -l`
if [ $errs -eq 0 ]
then
    rm output/$errfile
fi
   

rm -f out-util_check*csv
rm -f err-util_check*txt
rm -f output/err.util_check*
rm -f output/out.util_check*
rm -f output/log.util_check*
rm benchmark_batch.txt
