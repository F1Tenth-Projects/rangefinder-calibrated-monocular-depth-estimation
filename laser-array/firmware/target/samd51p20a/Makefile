TARGET_OBJS += target/$(TARGET)/startup_samd51.o
TARGET_OBJS += target/$(TARGET)/system_samd51.o

F_CPU = 120000000
CFLAGS += -DSAMD51
CHIP_DEFINE = __SAMD51P20A__

.PHONY: flash
flash:
	$(QUIET_FLASH) openocd -f target/$(TARGET)/openocd.cfg \
	                       -c 'program $(FW) verify reset exit'

.PHONY: metroflash
metroflash:
	bossac --offset=0x4000 -p /dev/ttyACM0 -e -w -v -R $(FW)

debug:
	openocd -f target/$(TARGET)/openocd.cfg
