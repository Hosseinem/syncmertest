#!/bin/bash
threshold=0.5
method=minimiser
kmer=**
smer=**

while getopts t:m:k:s: flag
do
    case "${flag}" in
        t) threshold=${OPTARG};;
        m) method=${OPTARG};;
        k) kmer=${OPTARG};;
        s) smer=${OPTARG};;
    esac
done

if [ $threshold = TRUE ];
then
    for file in *$method\_hash_$kmer\_$smer\_all_accuracy.out; \
    do \
        echo -n ${file%%_*};
        echo -n " , ";
        awk '{split($1,name,"_"); print name[3],",",name[4],",", $4,",", $5 }' $file; \
    done > data_$method\_threshold_$kmer\_$smer.csv;
else
    for file in $threshold\_$method\_hash_$kmer\_$smer\_all_accuracy.out; \
    do \
        awk '{split($1,name,"_"); print name[3],",",name[4],",", $4,",", $5 }' $file; \
    done > data_$method\_$threshold\_$kmer\_$smer.csv;
fi
