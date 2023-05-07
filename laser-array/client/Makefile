CC = gcc
CFLAGS  += -g -O3 -Wall -Iinclude -I/usr/include/libusb-1.0/ -fPIC \
           -I ../common/include
LDFLAGS += -lc

BINLIST   = liblaserarray.so laserarrayctl
CLEANLIST = $(BINLIST) $(wildcard *.o) unit-main.c

# target-specific objects in the form of 'target-objs = foo.o'
liblaserarray.so-objs := laserarray.o
liblaserarray.so-libs := -shared -lusb-1.0 -lpthread
liblaserarray.so-deps := include/laserarray.h

laserarrayctl-objs := laserarrayctl.o
laserarrayctl-deps := liblaserarray.so
laserarrayctl-libs := -L. -llaserarray -Wl,-rpath=.


all: $(BINLIST)

.SECONDEXPANSION:
$(BINLIST) : % : $$($$@-objs) $$($$@-deps)
	$(QUIET_LINK) $(CC) $(CFLAGS) $($@-objs) -o $@ $(LDFLAGS) $($@-libs)

%.o: %.c
	$(QUIET_CC) $(CC) $(CFLAGS) -c $< -o $@

define removefile
if [ -e "$(1)" ]; then \
printf "  %-7s  %s\n" "CLEAN" '$(1)'; \
rm -rf $(1); \
fi

endef

clean:
	@$(foreach file, $(CLEANLIST), $(call removefile,$(file)))

.SECONDARY:
.PHONY: clean all

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
QUIET_GEN  = @printf "  %-7s  %s\n" GEN $@;
QUIET_CC   = @printf "  %-7s  %s\n" CC $@;
QUIET_LINK = @printf "  %-7s  %s\n" LINK $@;
endif
endif
