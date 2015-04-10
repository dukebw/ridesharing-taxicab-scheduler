#!/bin/bash
for i in $(seq 5 8); do
    convert 15_915${i}_1194*.png -append col_${i}.png
done
convert col_* +append north_york_test.png
