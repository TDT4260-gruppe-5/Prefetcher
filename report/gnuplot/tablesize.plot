load "terminal.cfg"
set output "tablesize.tex"
set logscale x 2
set xtic rotate by -45
#plot "tablesize.data" with lp notitle
plot "tablesize.data" with lines lc 0 lt 1 notitle
