#!/bin/sh

file=results.txt
prevLine=prevLine
bool=true

cat $file | while read line
do
	first=$(echo $line | cut -d " " -f1)

	if [ ${#first} -le 4 ] && [ "$bool" = true ]; then
		echo $prevLine
		bool=false
	else
		if [ ! ${#first} -le 4 ]; then
			bool=true
		fi
	fi

	prevLine=$(echo $line     | cut -d " " -f4)
	prevLine=$(echo $prevLine | sed 's/://g')
done