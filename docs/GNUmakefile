PRESENTATION_SOURCES=$(wildcard *.qmd)
PRESENTATION_HTMLS=$(patsubst %.qmd,%.html,$(PRESENTATION_SOURCES))
PRESENTATION_PDFS=$(patsubst %.qmd,%.pdf,$(PRESENTATION_SOURCES))

.SUFFIXES: .qmd .html .pdf
.PHONY: clean

all: html
html: $(PRESENTATION_HTMLS)
pdf: $(PRESENTATION_PDFS)

.qmd.pdf:
	quarto render $< --to pdf

.qmd.html:
	quarto render $< --to html


clean:
	rm -f $(PRESENTATION_HTMLS)
	rm -f $(PRESENTATION_PDFS)
	rm -f *.qmd.bak *.tex *.log *.aux *.fdb_latexmk *.fls
	rm -rf local_minima_rastrigin_files/
	rm -rf local_minimima_rastrigin_files
