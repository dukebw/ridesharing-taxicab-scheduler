#!/bin/bash
for i in $(seq 5 8); do
    for j in $(seq 2 4); do
        wget "http://a.tile.openstreetmap.org/15/915"$i"/1194"$j".png" -O "15_915"$i"_1194"$j".png"
    done
done
