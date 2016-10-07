//
// C++ Implementation: Spi
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
extern "C"{
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <string.h>
}
#include "Defines.h"
#include "Spi.h"

//
// static class instance and members
//

Spi Spi::spi;
unsigned char Spi::cnt, Spi::flagCurVol;
unsigned char Spi::dpllEnable[3] = { 0x10, 0x10, 0x10};
volatile unsigned char Spi::outputCurrent[24],Spi::outputVoltage[24], Spi::outputStatus[3];
unsigned char Spi::outVal[24];
volatile lword Spi::outDirekt;

//
// table of phase connections
//

unsigned char const Spi::phaseTab[24] PROGMEM = {
    0, 1, 6, 7, 12, 13, 18, 19, 2, 3, 8, 9, 14, 15, 20, 21, 4, 5, 10,
    11, 16, 17, 22, 23
};
unsigned char const Spi::phasePtr[24] PROGMEM = {
    0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2, 0,
    0, 1, 1, 2, 2
};


Spi::Spi()
{
    PORTB = 0xFF;
    DDRB = 0x76; // set three SS + MOSI + SCK drivers to output
    SPCR = 0x57; // 1/128 CLK
}

void Spi::scan()
{
    unsigned char b;
	unsigned char a;
	
	
    if ( !cnt ) { // first 0x55
        PORTB &= ~(_BV(4)); //cbi( PORTB, 4 ); // Select R timebase
        b = SPSR;
        b = SPDR;
        if ( flagCurVol )			 SPDR = 0x0a|dpllEnable[0];		
        else 			 
			 SPDR = 0x05|dpllEnable[0]; // send first chn
    }
    else if ( ( cnt > 0 ) && ( cnt < 9 ) ) { // 8 data bytes
        b = SPSR;
        if ( !flagCurVol )
            outputCurrent[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 1] )] = SPDR; //
        // read output current
        else
            outputVoltage[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 1] )] = SPDR; //
        // read output voltage
        SPDR = outVal[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 1] )];
    }
    else if ( cnt == 9 ) {
        b = SPSR;
        outputStatus[0] = SPDR;
        SPDR = outDirekt.b.b0; // finish here with direkt flag
				
    }
    // second
    else if ( cnt == 10 ) { // second 0x55
        PORTB |= _BV(4);  //sbi( PORTB, 4 );
        PORTB &= ~(_BV(5));//cbi( PORTB, 5 ); // Select S Timebase
        b = SPSR;
        b = SPDR;
        if ( flagCurVol ) SPDR = 0x0a|dpllEnable[1];
        else SPDR = 0x55|dpllEnable[1]; // send first char
    }
    else if ( ( cnt > 10 ) && ( cnt < 19 ) ) { // 8 data bytes
        b = SPSR;
        if ( !flagCurVol )
            outputCurrent[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 3] )] = SPDR; //
        // read output current
        else
            outputVoltage[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 3] )] = SPDR; //
        // read output voltage
        SPDR = outVal[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 3] )];

    }
    else if ( cnt == 19 ) {
        b = SPSR;
        outputStatus[1] = SPDR;
        SPDR = outDirekt.b.b1; // finish here with direkt flag
    }
    // third
    else if ( cnt == 20 ) { // second 0x55
        PORTB |= _BV(5); //sbi( PORTB, 5 );
        PORTB &= ~(_BV(6));  //cbi( PORTB, 6 ); // Select T Timebase
        b = SPSR;
        b = SPDR;
        if ( flagCurVol ) SPDR = 0x0a|dpllEnable[2];
        else SPDR = 0x05|dpllEnable[2]; // send first char
    }
    else if ( ( cnt > 20 ) && ( cnt < 29 ) ) { // 8 data bytes
        b = SPSR;
        if ( !flagCurVol )
            outputCurrent[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 5] )] = SPDR; //
        // read output current
        else
            outputVoltage[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 5] )] = SPDR; //
        // read output current
        SPDR = outVal[pgm_read_byte_near( ( unsigned int ) & phaseTab[cnt - 5] )];
		
    }
    else if ( cnt == 29 ) {
        b = SPSR;
        outputStatus[2] = SPDR;
        SPDR = outDirekt.b.b2; // finish here with direkt flag
    }
    // finish here
    else if ( cnt == 30 ) {
        PORTB |= _BV(6);  //sbi( PORTB, 6 );
        flagCurVol = !flagCurVol;
    }
    if (++cnt>REFRESHTICK) cnt=0;
}
