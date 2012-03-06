REPORT = report.pdf
TEX_FILES = $(wildcard *.tex)

all : $(REPORT)

%.pdf : %.tex $(TEX_FILES)
	pdflatex $<
	pdflatex $<
	pdflatex $<

clean:
	rm $(REPORT)
