REPORT = report.pdf
PLOT_FILES = $(wildcard gnuplot/*.plot)
PLOT_OUTPUT = $(patsubst %.plot,%.tex,$(PLOT_FILES))
TEX_FILES = $(wildcard *.tex) $(PLOT_OUTPUT)

all : $(REPORT)

%.pdf : %.tex $(TEX_FILES) bibliography.bib
	pdflatex $<
	bibtex $(basename $<)
	pdflatex $<
	pdflatex $<

$(PLOT_OUTPUT) : gnuplot

.PHONY : gnuplot
gnuplot:
	$(MAKE) -C gnuplot

.PHONY : clean
clean:
	rm $(REPORT)
