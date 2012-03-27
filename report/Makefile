REPORT = report.pdf
TEX_FILES = $(wildcard *.tex)

all : $(REPORT)

%.pdf : %.tex $(TEX_FILES)
	pdflatex $<
	bibtex $(basename $<)
	pdflatex $<
	pdflatex $<

clean:
	rm $(REPORT)
