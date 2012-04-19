load "terminal.cfg"
#set termoption solid

set output "mcs.tex"
set xtics 1
set xlabel "Aggressivness"
set ylabel "Average speedup"
set yrange [1.088: 1.103]
#plot for [i=0:4] "mcs.data" index i with lines ls (i + 1) title columnheader(1)
plot for [i=0:4] "mcs.data" index i with lines lc 0 lt (i + 1) title columnheader(1)
