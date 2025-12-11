SDK_HOME = /home/jeyoung/source/github/ESP8266_NONOS_SDK
CC = /usr/bin/xtensa-lx106-elf-gcc
AR = /usr/bin/xtensa-lx106-elf-ar
LD = /usr/bin/xtensa-lx106-elf-ld
CFLAGS = -I. -I./lib -I$(SDK_HOME)/include -I$(SDK_HOME)/third_party/include -I$(SDK_HOME)/driver_lib/driver -I$(SDK_HOME)/driver_lib/include/driver -mlongcalls -Wall -Wl,--verbose
LDLIBS = -nostdlib -static -Wl,--start-group -lc -lgcc -ldriver -lnet80211 -lwpa -llwip -lmain -lpp -lphy -lmbedtls -lssl -lcrypto -lwps -Wl,--end-group -lmain
LDFLAGS = -L. -L$(SDK_HOME)/lib -T$(SDK_HOME)/ld/eagle.app.v6.ld --sysroot=$(SDK_HOME)/ld $(LDLIBS)
PROGRAM = main

ESPTOOL = esptool
BAUDRATE = 921600

.PHONY:
all: clean $(PROGRAM)

$(PROGRAM)-0x00000.bin: $(PROGRAM)
	$(ESPTOOL) elf2image $^

$(PROGRAM): $(PROGRAM).o

$(PROGRAM).o: $(PROGRAM).c

flash: $(PROGRAM)-0x00000.bin
	$(ESPTOOL) -b $(BAUDRATE) write_flash 0x0 $(PROGRAM)-0x00000.bin 0x10000 $(PROGRAM)-0x10000.bin

clean:
	$(RM) -f $(PROGRAM) $(PROGRAM).o $(PROGRAM)-0x00000.bin $(PROGRAM)-0x10000.bin lib/*.o
	$(RM) -f libbutton.a
