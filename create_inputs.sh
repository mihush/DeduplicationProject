#!/bin/bash

printf -v input_files_dir "/home/polinam/07_03_18/"
# input_files_dir = /home/shared/storage-simulations/org-traces/UBC-Dedup/64r/

## ------------- Single System 1 through 3 -------------
#### Block Level Deduplication
for((i = 2 ; $i < 3 ; i++)); do
	printf "%04d\n" $i
	printf -v var "%04d\n" $i
	printf -v fileName "input_params"
	echo $fileName
	touch $fileName
	echo B >> $fileName
	echo $input_files_dir >> $fileName
	echo 1 >> $fileName
	echo $var >> $fileName
	### Call a.out here
	time ./a.out
	rm $fileName
done


#### File Level Deduplication
for((i = 2 ; $i < 3 ; i++)); do
	printf "%04d\n" $i
	printf -v var "%04d\n" $i
	printf -v fileName "input_params"
	echo $fileName
	touch $fileName
	echo F >> $fileName
	echo $input_files_dir >> $fileName
	echo 1 >> $fileName
	echo $var >> $fileName
	### Call a.out here
	time ./a.out
	rm $fileName
done

