#!/bin/bash

gnuplot << EOF
reset
set tics scale 0
set format y '%.0f'
set format x '%.0f'
set terminal pdfcairo enhanced color
set output "profile_$1.pdf"
set style line 101 lc rgb '#000000' lt 1 lw 4
set border 3 front ls 101
set tics nomirror out scale 0.75
set format '%g'
set size ratio 0.3
unset key
set yrange [:] 
set xrange [:"$(wc -l "$1" | cut -d' ' -f1)"]
set xtics auto
set ytics 0.1
set grid 
set ylabel "Bps"
set xlabel "Length"
set border linewidth 1.5
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 5 ps 0.4
plot "$1" using 1 with lines ls 1
EOF

