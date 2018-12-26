set boxwidth 0.8 absolute
set style fill solid 1.00 border -1
set style histogram clustered gap 1 title offset character 0, 0, 0
set datafile missing '-'
set style data histograms

plot './data.dat' using 2:xtic(1) ti col ls 1, '' u 3 ti col ls 2, '' u 5 ti col ls 3, '' u 6 ti col ls 5

set terminal postscript eps enhanced color

set style line 1 lt 1 lc 3 lw 3
set style line 2 lt 1 lc 2 lw 3
set style line 3 lt 1 lc 1 lw 3
set style line 4 lt 3 lc rgb "gray" lw 0.3
set style line 5 lt 1 lc 5 lw 3
set style line 6 lt 1 lc 5 lw 3


set ylabel "Normalized average energy efficiency" font "Helvetica,20" offset -1.8,0
set ytics ("0.2" 0.2, "0.4" 0.4, "0.6" 0.6, "0.8" 0.8, "1" 1, "1.2" 1.2) font "Helvetica,20"
set xrange [-0.7:3.7]
set yrange [0:1.52]
set grid ls 4
set grid noxtics
set xtics nomirror
set size 1,0.8
set lmargin 12
set rmargin 5
set tmargin 2
set bmargin 5
set key at 2.5,1.52 Left width -2
set key top horizontal right
set key font "Helvetica,20"
set key spacing 1.3
set key samplen 2
set key reverse

set output './pic.eps'
replot
