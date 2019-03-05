#!/bin/sh

COUNTER=0

for img in *.jp2
do
	convert $img "bg_$COUNTER.jpg"

	COUNTER=$((COUNTER+1))
done