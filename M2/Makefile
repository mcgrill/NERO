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
OBJECTS    = milkshake.o m_usb.o m_bus.o 

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
all:	milkshake.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

install: flash 

flash: all
	dfu-programmer atmega32u4 erase
	dfu-programmer atmega32u4 flash milkshake.hex

clean:
	rm -f milkshake.hex milkshake.elf $(OBJECTS)

# file targets:
milkshake.elf: $(OBJECTS)
	$(COMPILE) -o milkshake.elf $(OBJECTS) $(LIBRARIES)

milkshake.hex: milkshake.elf
	rm -f milkshake.hex
	avr-objcopy -j .text -j .data -O ihex milkshake.elf milkshake.hex

# Targets for code debugging and analysis:
disasm:	milkshake.elf
	avr-objdump -d milkshake.elf

cpp:
	$(COMPILE) -E milkshake.c
