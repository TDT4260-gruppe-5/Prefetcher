load "terminal.cfg"
set output "aggr.tex"
#plot "tablesize.data" with lp notitle
plot "aggr.data" using 2:xticlabels(1) with lines lc 0 lt 1 notitle
