//
// C++ Implementation: Adc
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
}

#include "Adc.h"

//
// static class instance and members
//

Adc Adc::adc;
volatile unsigned int Adc::rms[3];
volatile unsigned int Adc::val[3];
unsigned char Adc::ms;

Adc::Adc()
{
}

void Adc::scan()
{
    ADMUX = 0xC0; // internal 2.56V reference, chan 0
    ADCSR = 0xCF; // prescaler 128, start
}    

void Adc::adcInt()
{
    unsigned char ad = ADMUX & 0x07;
    if ( ms < 10 ) {
        unsigned char sreg = SREG; // save interrupt state
        cli(); // disable
        val[ad] += ADC;
        SREG = sreg; // restore interrupt state
        if ( ad == 2 ) ms++;
    }
    else {
        rms[ad] = val[ad];
        val[ad] = 0;
        if ( ad == 2 ) ms = 0;
    }
    ADMUX = 0xC0 | ( ++ad );
    if ( ad < 3 ) ADCSR = 0xCF;
    else ADCSR = 0x66;
}

ISR( ADC_vect ) // use interrupt here to be interruptable
{
       Adc::adcInt();
}

