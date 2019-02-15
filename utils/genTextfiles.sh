#!/bin/sh

# Create empty annotation file for each image (000.jpg -> 000.txt etc.)

for f in *.jpg
do
    base=${f}
    filename="${base%.*}"

    # Warning: First give it a shot with echo instad of touch
    touch $filename.txt
done