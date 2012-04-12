load "terminal.cfg"
set output "mcs.tex"
set xtics 1
set xlabel "count"
set ylabel "Average speedup"
#plot for [i=0:4] "mcs.data" index i with linespoints ls (i + 1) title columnheader(1)
plot for [i=0:4] "mcs.data" index i with lp lc 0 lt (i + 1) title columnheader(1)
