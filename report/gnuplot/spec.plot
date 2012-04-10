load "terminal.cfg"
set output "spec.tex"
set termoption solid

set style data histograms
set style histogram clustered gap 2

set linetype 1 lc "black"
set linetype 2 lc "black"

#set style fill solid 1.0 border lt -1
set style fill pattern 2

set xtic rotate by -45
set yrange[0.8:1.8]
set ylabel "Speedup"

plot for [COL=3:2:-1] "spec.data" using COL:xtic(1) title columnheader(COL)
