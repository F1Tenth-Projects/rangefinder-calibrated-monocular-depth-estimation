ADOC = asciidoctor-pdf
ADOCFLAGS = -a pdf-theme=theme.yml -r ./bibliography.rb

TEX = pdflatex
TEXFLAGS = -halt-on-error -output-directory $(dir $@)

DOC_LIST = proposal.pdf
DOC_LIST = proposal.pdf final-report.pdf
CLEAN_LIST += $(DOC_LIST) schedule.png block-diagram.png \
              proposal.aux proposal.log \
              final-report.aux final-report.log

proposal.pdf-deps := block-diagram.png schedule.png
fianl-report.pdf-deps := block-diagram.png schedule.png

docs: $(DOC_LIST)

.PHONY: clean
clean:
	rm -f $(CLEAN_LIST)


.SECONDEXPANSION:
%.pdf: %.adoc $$($$@-deps)
	$(QUIET_ADOC) $(ADOC) $(ADOCFLAGS) $< -o $@

.SECONDEXPANSION:
%.pdf: %.tex $$($$@-deps)
	$(QUIET_TEX) $(TEX) $(TEXFLAGS) $< -o $@ | (! grep \!)

%.png: %.puml
	$(QUIET_GEN) plantuml -Tpng $<

%.png: %.svg
	$(QUIET_GEN) inkscape --export-filename=$@ --export-type=png $<

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
QUIET_ADOC    = @printf "  %-7s  %s\n" ADOC $@;
QUIET_TEX     = @printf "  %-7s  %s\n" TEX $@;
QUIET_GEN     = @printf "  %-7s  %s\n" GEN $@;
endif
endif
