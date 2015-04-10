#!/bin/bash
for i in $(seq 77 80); do
    convert 14_45${i}_597*.png -append col_${i}.png
done
convert col_* +append north_york_test.png
