#!/bin/bash

./subtract.py "$1.iae_AC" "$1.iae_AC2" > "$1_AC-AC2" && ./plot_profile.sh "$1_AC-AC2"
