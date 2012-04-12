PLOT_FILES = $(wildcard *.plot)
TEX_FILES = $(patsubst %.plot,%.tex,$(PLOT_FILES))

all: $(TEX_FILES)

%.tex : %.plot %.data terminal.cfg
	gnuplot -e "load '$<'"
