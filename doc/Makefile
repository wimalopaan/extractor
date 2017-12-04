# -*- mode: makefile-gmake; -*-

ASCIIDOC = asciidoc
ASCIIDOCTOR = asciidoctor

#ASCIIDOCTORPDF = ~/asciidoctor/asciidoctor-pdf/bin/asciidoctor-pdf

# rendered admonitions als image
ASCIIDOCTORPDF = ~/asciidoctor/jessedoyle-asciidoctor-pdf/bin/asciidoctor-pdf

EXTENSIONSDIR = ~/asciidoctor/extensions/lib 

ASCIIDOCTORFLAGS += -I $(EXTENSIONSDIR) -r view-result-postprocessor -r man-inline-macro -r asciidoctor-diagram -r tree-block-macro
ASCIIDOCTORFLAGSPDF += -I $(EXTENSIONSDIR)  -r asciidoctor-diagram -r tree-block-macro
ASCIIDOCTORVARS += -a data-uri -a stylesdir=css -a stylesheet=asciidoctor-wide.css

#SCREENSHOT = ~/asciidoctor/asciidoctor-screenshot/buildSrc/src/main/groovy/org/asciidoctor/extension
#ASCIIDOCTORFLAGS += -I $(SCREENSHOT) -r ScreenshotExtensionRegistry.groovy

WK = ~/Software/wkhtmltopdf/wkhtmltox-0.12.2.1/static-build/wkhtmltox-0.12.2.1/bin/wkhtmltopdf
WKFLAGS += --footer-line --footer-left "Prof. Dr.-Ing. W. Meier" --footer-right "[page] - [topage]"  --footer-center "[date]"
WKFLAGS += --header-line --header-left "[section]" --header-center "[subsection]" --header-right "[subsubsection]" 
WKFLAGS += --header-spacing 5 --footer-spacing 5 
WKFLAGS += --header-font-size 9 --footer-font-size 9 
WKFLAGS += -T 15 -B 15 
WKFLAGS +=  --enable-toc-back-links
WKLANDSCAPE += -O landscape 
WKPORTRAIT += -O portrait

NUP = pdfnup
NUPFLAGSL += -q --nup 1x2 --no-landscape --keepinfo --frame true
NUPFLAGSP += -q --nup 2x1 --keepinfo --frame true

all: examples html

examples:
	make -C src all

WBE = ../web.extract

web:
	touch $(WBE)
	find . -name '*.html' -exec cp -a --parents {} $(WBE) \;
	cp $(WBE)/extractor.html $(WBE)/index.html

sources += extractor.adoc

.PHONY: html pdf clean examples

HTML += $(patsubst %.adoc, %.html, $(sources))
#HTML += $(patsubst %.adoc, %_p.html, $(sources))

html: $(HTML) 

PDF += $(patsubst %.adoc, %_w.pdf, $(sources))
PDF += $(patsubst %.adoc, %_w2.pdf, $(sources))
PDF += $(patsubst %.adoc, %_wp.pdf, $(sources))
PDF += $(patsubst %.adoc, %_wp2.pdf, $(sources))

pdf: $(PDF)

%_p.html: %.adoc
	$(ASCIIDOCTOR) $(ASCIIDOCTORFLAGS) $(ASCIIDOCTORVARS) -a deckjs -a split -T $(HOME)/asciidoctor/asciidoctor-backends/haml/deckjs -o $@ $<

%_r.html: %.adoc
	$(ASCIIDOCTOR) $(ASCIIDOCTORFLAGS) $(ASCIIDOCTORVARS) -a revealjs -a split -b revealjs -T $(HOME)/asciidoctor/asciidoctor-reveal.js/templates/slim -o $@ $<

%.html: %.adoc
	$(ASCIIDOCTOR) $(ASCIIDOCTORFLAGS) $(ASCIIDOCTORVARS) -a single -o $@ $<

%.xml: %.adoc
	$(ASCIIDOCTOR) -b docbook5 -a print -r asciidoctor-diagram -o $@ $<

%.foc: %.xml
	xsltproc --nonet $(HOME)/asciidoctor/xsl/codeblocks.xsl $< > $@

%.fo: %.foc
	java -classpath /usr/share/java/docbook-xsl-saxon/docbook-xsl-saxon.jar:/usr/share/java/saxon6/saxon.jar:/usr/local/share/java/xslthl-2.1.0.jar -Dxslthl.config=file:///usr/local/share/java/highlighters/xslthl-config.xml com.icl.saxon.StyleSheet -o $@ $< $(HOME)/asciidoctor/xsl/fo.xsl

%_draft.fo: %.foc
	java -classpath /usr/share/java/docbook-xsl-saxon/docbook-xsl-saxon.jar:/usr/share/java/saxon6/saxon.jar:/usr/local/share/java/xslthl-2.1.0.jar -Dxslthl.config=file:///usr/local/share/java/highlighters/xslthl-config.xml com.icl.saxon.StyleSheet -o $@ $< $(HOME)/asciidoctor/xsl/fo-simple.xsl

%.pdf: %.fo
	fop $< $@

%_r.pdf: %.adoc
	$(ASCIIDOCTORPDF) $(ASCIIDOCTORFLAGSPDF) $(ASCIIDOCTORVARS) -a pagenums -o $@ $<

%_w.pdf: %.html
	$(WK) $(WKFLAGS) $(WKLANDSCAPE) $< $@
	
%_w2.pdf: %_w.pdf 
	$(NUP) $(NUPFLAGSL) -o $@ $<

%_wp.pdf: %.html
	$(WK) $(WKFLAGS) $(WKPORTRAIT) $< $@
	
%_wp2.pdf: %_wp.pdf 
	$(NUP) $(NUPFLAGSP) -o $@ $<

clean:
	$(RM) *~
	$(RM) $(HTML)
	$(RM) $(PDF)
	$(RM) *.pdfmarks
	$(RM) *.fo
	$(RM) *.foc
	$(RM) *.xml
	$(RM) diag*.png
	$(RM) diag*.png.cache
	$(RM) diag*.svg
	$(RM) diag*.svg.cache

