	    CC = avr-gcc
	    AS = avr-as
	    LD = avr-ld
	LFLAGS = -L/usr/avr/lib/avr5 -mmcu=atmega128 -Wl,-Map=Dmx.map,--cref
	CFLAGS = -g -O2 -Wall -I/usr/avr/include -mmcu=atmega128
	CPPFLAGS = -g -O2 -Wall -fno-exceptions -I/usr/avr/include -mmcu=atmega128

all:Dmx.elf

%.o : %.c
	avr-gcc -c $(CFLAGS) $<

%.o : %.cpp
	avr-g++ -c $(CPPFLAGS) $<

App.o:  App.cpp App.h Defines.h Lcd.h Kbd.h Nvr.h Scan.h Dmx.h Adc.h Menue.h
Dmx.o:	Dmx.cpp Dmx.h Defines.h Nvr.h
Lcd.o:  Lcd.cpp Lcd.h Kbd.h
Nvr.o:  Nvr.cpp Nvr.h Defines.h
Curve.o:  Curve.cpp Curve.h Nvr.h Dmx.h Defines.h
Kbd.o:  Kbd.cpp Kbd.h Defines.h
Adc.o:  Adc.cpp Adc.h
Spi.o:  Spi.cpp Spi.h Defines.h Nvr.h
Scan.o:  Scan.cpp Scan.h Defines.h Kbd.h Adc.h Spi.h Dmx.h Nvr.h Curve.h Menue.h
Menue.o: Menue.cpp Menue.h Lcd.h Kbd.h Nvr.h Adc.h Dmx.h Curve.h Scan.h Defines.h
Dda.o:	Dda.cpp Dda.h Defines.h

Dmx.elf: App.o Dmx.o Lcd.o Nvr.o Curve.o Kbd.o Adc.o Spi.o Scan.o Menue.o Dda.o
	avr-g++ -o Dmx.elf -g App.o Dmx.o Lcd.o Nvr.o Curve.o Kbd.o Adc.o Spi.o Dda.o \
	 Scan.o Menue.o -Wl,-Map=Dmx.map,--cref $(LFLAGS)
#	avr-objdump -h Dmx.elf
	$(LD) -mavr5 --oformat=ihex Dmx.elf -o Dmx.hex
	$(LD) -mavr5 --oformat=binary Dmx.elf -o Dmx.bin

prg:
	$(LD) -mavr5 --oformat=srec Dmx.elf -o Dmx.srec
	uisp -dprog=dapa --erase
	uisp -dprog=dapa --upload if=Dmx.srec -v=3
init:
	uisp -dprog=dapa --segment=fuse --upload if=fuse.srec

avarice:
	avarice -f Dmx.hex --erase --program

clean:
	rm -f *.o *.elf t *.i.* *.i *.c.*

rebuild: clean all
