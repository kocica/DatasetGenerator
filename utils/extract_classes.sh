#!/bin/sh

for file in BTSD/dataset/*.txt
do
	cat $file | while read line
	do
		echo $(echo $line) | cut -d " " -f1
	done
done