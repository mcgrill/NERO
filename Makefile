# --------------------------------------------------------
# Custom M2 Makefile
# written by: Jonathan Bohren & Jonathan Fiene
# updated: July 6, 2011
# --------------------------------------------------------

# --------------------------------------------------------
# if you write separate C files to include in BetaCode
# add their .o targets to the OBJECTS line below
# (e.g. "OBJECTS = BetaCode.o myfile.o")
# --------------------------------------------------------
OBJECTS    = dooty_test.o m_usb.o m_bus.o 

# --------------------------------------------------------
# if you need to use one of our pre-compiled libraries,
# add it to the line below (e.g. "LIBRARIES = libsaast.a")
# --------------------------------------------------------
LIBRARIES  = libsaast.a

# --------------------------------------------------------
# Default settings for the M2:
# --------------------------------------------------------
DEVICE     = atmega32u4
CLOCK      = 16000000

# --------------------------------------------------------
# you shouldn't change anything below here,
# unless you really know what you're doing
# --------------------------------------------------------

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all:	dooty_test.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

install: flash 

flash: all
	dfu-programmer atmega32u4 erase
	dfu-programmer atmega32u4 flash dooty_test.hex

clean:
	rm -f dooty_test.hex dooty_test.elf $(OBJECTS)

# file targets:
dooty_test.elf: $(OBJECTS)
	$(COMPILE) -o dooty_test.elf $(OBJECTS) $(LIBRARIES)

dooty_test.hex: dooty_test.elf
	rm -f dooty_test.hex
	avr-objcopy -j .text -j .data -O ihex dooty_test.elf dooty_test.hex

# Targets for code debugging and analysis:
disasm:	dooty_test.elf
	avr-objdump -d dooty_test.elf

cpp:
	$(COMPILE) -E dooty_test.c
