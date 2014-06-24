all: doc.pdf report.pdf

doc.pdf: doc.tex
	xelatex doc && bibtex doc && xelatex doc && xelatex doc

report.pdf: report.tex
	xelatex report && xelatex report

clean: 
	rm -f *.aux *.log *.nav *.snm *.out *.vrb *.toc *.blg *.bbl
