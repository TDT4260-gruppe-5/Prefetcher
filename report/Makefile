REPORT = report.pdf
TEX_FILES = $(wildcard *.tex)

all : $(REPORT)

%.pdf : %.tex $(TEX_FILES) bibliography.bib
	pdflatex $<
	bibtex $(basename $<)
	pdflatex $<
	pdflatex $<

clean:
	rm $(REPORT)
