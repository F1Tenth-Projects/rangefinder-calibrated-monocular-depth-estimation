TEX = pdflatex
TEXFLAGS = -halt-on-error -output-directory $(dir $@)

DOC_LIST = proposal.pdf final-report.pdf
CLEAN_LIST += $(DOC_LIST) schedule.png block-diagram.png \
              block-diagram-final.png\
              proposal.aux proposal.log \
              final-report.aux final-report.log

proposal.pdf-deps := block-diagram.png schedule.png
final-report.pdf-deps := block-diagram-final.png cables/laser-array-cables.png

docs: $(DOC_LIST)

.PHONY: clean
clean:
	rm -f $(CLEAN_LIST)


.SECONDEXPANSION:
%.pdf: %.tex $$($$@-deps)
	$(QUIET_TEX) $(TEX) $(TEXFLAGS) $< -o $@ | (! grep \!)

%.png: %.puml
	$(QUIET_GEN) plantuml -Tpng $<

%.png: %.yaml
	$(QUIET_GEN) wireviz $<

%.png: %.svg
	$(QUIET_GEN) inkscape --export-filename=$@ --export-type=png $<

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
QUIET_ADOC    = @printf "  %-7s  %s\n" ADOC $@;
QUIET_TEX     = @printf "  %-7s  %s\n" TEX $@;
QUIET_GEN     = @printf "  %-7s  %s\n" GEN $@;
endif
endif
