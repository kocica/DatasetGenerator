#!/bin/bash

for weight in ./auto_eval/weights/*; do
    echo "$weight"
    mkdir ./auto_eval/results/$(basename $weight)
    ./darknet detector test -thresh 0.5 cfg/obj.data cfg/yolo-train.cfg "$weight" -ext_output -dont_show < ~/Desktop/BTSD_small.txt > results.txt
    ./gen_eval.sh
    mv ./mAP_Gen/results/* ./auto_eval/results/$(basename $weight)
done