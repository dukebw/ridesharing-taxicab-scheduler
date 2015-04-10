#!/bin/bash
for i in $(seq 77 80); do
    for j in $(seq 1 3); do
        wget "http://a.tile.openstreetmap.org/14/45"$i"/597"$j".png" -O \
        "14_45"$i"_597"$j".png"
    done
done
