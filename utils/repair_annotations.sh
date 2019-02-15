#!/bin/sh

# Repair f*ed up annotations

for file in RTSD/dataset/*.txt
do
	cat $file | while read line
	do
		c=$(echo $line | cut -d' ' -f1)
		x=$(echo $line | cut -d' ' -f2)
		y=$(echo $line | cut -d' ' -f3)
		w=$(echo $line | cut -d' ' -f4)
		h=$(echo $line | cut -d' ' -f5)
		X=1280 # TODO: Width
		Y=720  # TODO: Height


		new_x=$(echo "(((${x} * ${X}) + ((${w} * ${X}) / 2)) / ${X})" | bc -l)
		new_y=$(echo "(((${y} * ${Y}) + ((${h} * ${Y}) / 2)) / ${Y})" | bc -l)

		new_x=$(echo $new_x | awk ' sub("\\.*0+$","") ')
		new_y=$(echo $new_y | awk ' sub("\\.*0+$","") ')

		if [ -z "$data" ]; then
			data="${c} 0${new_x} 0${new_y} ${w} ${h}"
		else
			data="${data}\n${c} 0${new_x} 0${new_y} ${w} ${h}"
		fi

		rm -f $file
		touch $file
		echo $data > $file
	done
done