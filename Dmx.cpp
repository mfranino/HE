//
// C++ Implementation: Dmx
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
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <string.h>
}

#include "Defines.h"
#include "Dmx.h"
#include "Nvr.h"

//
// Static class instances and members
//

Dmx Dmx::dmx;
volatile unsigned char Dmx::inBuff1[24], Dmx::inBuff2[24], Dmx::inVal[24]; // inVal[24] -> chan values after dmx merge// rmsVal[24] -> chan values after phisiological curve // outVal[24] -> chan values after the RMS curve
volatile unsigned char Dmx::valid ,Dmx::valid1, Dmx::valid2;
volatile unsigned char Dmx::first1,Dmx::scan1,Dmx::read1, Dmx::flag1;
volatile unsigned char Dmx::first2,Dmx::scan2,Dmx::read2,Dmx::flag2;
volatile unsigned char * Dmx::ptr1, * Dmx::ptr2;
volatile unsigned int Dmx::cnt1,Dmx::cnt2;
unsigned char Dmx::mode = 0;  // 0=1only, 1=2 only, 2=merge both
/*volatile unsigned char  Dmx::valid;*/

Dmx::Dmx()
{
    // Dmx 1
    UBRR0L = ( MEGACLOCK / 4 - 1 ); // 250kbaud
    UCSR0A = 0;
    UCSR0B = _BV( RXCIE ) | _BV( RXEN ) | _BV( UCSZ2 ) | _BV( TXB8 );
                                                 // 9 bit rxen rxinten
    UCSR0C = _BV( UCSZ1 ) | _BV( UCSZ0 ); // 9 bit 1 stop
    // Dmx 2
    PORTD &= ~(_BV(4));  //     cbi( PORTD, 4 );
	DDRD |= _BV(4);   //     sbi( DDRD, 4 );                 // input


    UBRR1L = ( MEGACLOCK / 4 - 1 ); // 250kbaud
    UCSR1A = 0;
    UCSR1B = _BV( RXCIE ) | _BV( RXEN ) | _BV( TXEN ) | _BV( UCSZ2 ) | _BV( TXB8 ); 
                                                 // 9 bit rxen rxinten
    UCSR1C = _BV( UCSZ1 ) | _BV( UCSZ0 ); // 9 bit 1 stop
}

void Dmx::dmx1Int()
{
    unsigned char stata = UCSR0A;
    unsigned char statb = UCSR0B;
    unsigned char data = UDR0;
    if ( !data && !( statb & _BV( RXB8 ) ) ) { // handle break here
        first1 = 1;
        scan1 = 0;
        read1 = 0;
        return;
    }
    if ( first1 ) {
        first1 = 0;
        if ( data ) return; // check break code
        flag1 = 1;
        scan1 = 1;
        ptr1 = &inBuff1[0];
        cnt1 = 0;
        data=stata;
        return;
    }
    if ( scan1 ) {
        if ( ( ++cnt1 ) >= Nvr::setup.dmxAddr ) {
            cnt1 = 0;
            scan1 = 0;
            read1 = 1;
        }
        else
            return;
    }
    if ( read1 ) {
        * ptr1++ = data;
        if ( ++cnt1 > 23 ) {
            valid1=30;
            read1 = 0;
        }
    }
}

extern "C"
ISR( USART0_RX_vect)
{
    Dmx::dmx1Int();
}

void Dmx::dmx2Int()
{
	    unsigned char stata = UCSR1A;
	    unsigned char statb = UCSR1B;
	    unsigned char data = UDR1;
	    if ( !data && !( statb & _BV( RXB8 ) ) ) { // handle break here
        first2 = 1;
        scan2 = 0;
        read2 = 0;
        return;
    }
    if ( first2 ) {
        first2 = 0;
        if ( data ) return; // check break code
        flag2 = 1;
        scan2 = 1;
        ptr2 = &inBuff2[0];
        cnt2 = 0;
        data=stata;
        return;
    }
    if ( scan2 ) {
        if ( ( ++cnt2 ) >= Nvr::setup.dmxAddr ) {
            cnt2 = 0;
            scan2 = 0;
            read2 = 1;
        }
        else
            return;
    }
    if ( read2 ) {
        * ptr2++ = data;
        if ( ++cnt2 > 23 ) {
            valid2=30;
            read2 = 0;
        }
    }
}

extern "C"
ISR( USART1_RX_vect )
{
    Dmx::dmx2Int();
}

void Dmx::merge()            // merge here
{
    unsigned char chn;
	
    if ( !mode ) {		
        if (valid=valid1)
            for ( chn = 0; chn < 24; chn++ )
                inVal[chn]=inBuff1[chn];
    }
	
    else if ( mode == 1 ) {
        if (valid=valid2)
            for ( chn = 0; chn < 24; chn++ ) {
                inVal[chn]=inBuff2[chn];
        }
    }
    else {
        if (valid= (valid1 || valid2))
            for ( chn = 0; chn < 24; chn++ ) {
                if ( inBuff1[chn] > inBuff2[chn] )
                    inVal[chn]=inBuff1[chn];
                else
                    inVal[chn]=inBuff2[chn];
            }
    }
    if (valid1) valid1--;
    if (valid2) valid2--;

}
//eof
