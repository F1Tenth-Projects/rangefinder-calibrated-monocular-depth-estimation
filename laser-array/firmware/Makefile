TARGET := samd51p20a

HOSTCC = gcc
HOSTCFLAGS = -O2 -Wall
HOSTLDFLAGS = -lc

CC = arm-none-eabi-gcc
CFLAGS  += -g -O2 -Wall -D$(CHIP_DEFINE) -mthumb -mcpu=cortex-m4 -g \
           -Iinclude -Iinclude/$(TARGET) -Itarget/$(TARGET)/include \
           -I ../common/include/ \
           -ffunction-sections -fdata-sections -fno-strict-aliasing \
           -DF_CPU=$(F_CPU)
LDFLAGS += -Ttarget/$(TARGET)/link.ld \
           -Wl,--gc-sections -static -specs=nano.specs

ELFLIST   = laser-array.elf
BINLIST   = $(ELFLIST:.elf=.bin)
CLEANLIST = $(BINLIST) $(ELFLIST) \
            $(wildcard *.o) $(wildcard target/$(TARGET)/*.o)

all: $(BINLIST)

-include target/$(TARGET)/Makefile
-include vl53lx/Makefile

# target-specific objects in the form of 'target-objs = foo.o'
laser-array.elf-objs := $(TARGET_OBJS) $(VL53LX_OBJS) laser-array.o i2c.o \
                        usb-descriptors.o usb.o panic.o sensor.o gpio.o

.SECONDEXPANSION:
$(ELFLIST) : % : $$($$@-objs)
	$(QUIET_LINK) $(CC) $(CFLAGS) $($@-objs) -o $@ $(LDFLAGS) $($@-libs)

%.bin: %.elf
	$(QUIET_OBJCOPY) arm-none-eabi-objcopy -O binary $< $@

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
QUIET_HOSTCC = @printf "  %-7s  %s\n" HOSTCC $@;
QUIET_HOSTLD = @printf "  %-7s  %s\n" HOSTLD $@;
QUIET_CC   = @printf "  %-7s  %s\n" CC $@;
QUIET_LINK = @printf "  %-7s  %s\n" LINK $@;
QUIET_OBJCOPY = @printf "  %-7s  %s\n" OBJCOPY $@;
endif
endif
