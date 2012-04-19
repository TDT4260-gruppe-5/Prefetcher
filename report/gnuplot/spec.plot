load "terminal.cfg"
set output "spec.tex"
set termoption solid

set style data histograms
set style histogram clustered gap 2

set linetype 1 lc "black"
set linetype 2 lc "black"
set linetype 3 lc "black"
set linetype 4 lc "black"
set linetype 5 lc "black"

#set style fill solid 1.0 border lt -1
set style fill pattern 2

set xtic rotate by -45
set yrange[0.8:1.8]

plot for [COL=5:2:-1] "spec.data" using COL:xtic(1) title columnheader(COL)
