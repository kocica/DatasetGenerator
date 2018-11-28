#!/bin/sh

# Create empty annotation files (000.jpg -> 000.txt)

dir = "."
for f in "$dir"*.jpg
do
    base=${f#$dir}
    filename="${base%.*}"
    touch $filename.txt
done


