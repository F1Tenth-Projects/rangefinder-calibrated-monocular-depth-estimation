ADOC = asciidoctor-pdf
ADOCFLAGS = -a pdf-theme=theme.yml

TEX = pdflatex
TEXFLAGS = -halt-on-error -output-directory $(dir $@)

DOC_LIST = proposal.pdf
CLEAN_LIST += $(DOC_LIST) proposal.aux proposal.log


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

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
QUIET_ADOC    = @printf "  %-7s  %s\n" ADOC $@;
QUIET_TEX     = @printf "  %-7s  %s\n" TEX $@;
QUIET_GEN     = @printf "  %-7s  %s\n" GEN $@;
endif
endif
