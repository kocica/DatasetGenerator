#!/bin/bash

# Script to rename classes in dozens of annotation files

COUNTER=0

cat classes.txt | while read line
do
	perl -pi -e "s/$line /$COUNTER /g" ./RTSD-01/*.txt

	# In case of argument list too long error:
	# find ./RTSD-01/ -type f -name '*.txt' -print0 | xargs -0 perl -pi -e "s/$line /$COUNTER /g"

	COUNTER=$[$COUNTER +1]
done