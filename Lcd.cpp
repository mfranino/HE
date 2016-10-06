//
// C++ Implementation: Lcd
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
#include <stdarg.h>
}
#define RW 0
#define RS 0
#define EN 7
#include "Lcd.h"
#include "Kbd.h"
//
// Static class instances and members
//

Lcd Lcd::lcd;
FILE *Lcd::console;

//
// needed for printf_P
//

extern "C" {
    int _putChar(char c) {
        Lcd::write((unsigned char) c);
        return 0;
    }
}

Lcd::Lcd()
{
    console = fdevopen(_putChar, _getChar, 0);
    PORTA = 0xff;               // pullups for data
    DDRA = 0;
    cbi(PORTC, EN);
    sbi(DDRC, EN);              // set EN pin to output
    sbi(PORTG, RW);
    sbi(DDRG, RW);              // set RW to read
    cbi(PORTC, RS);
    sbi(DDRC, RS);              // set RS to 0
    for (unsigned char i = 0; i < 6; i++) {
        setInterface();
    }
    outCommand(0x38);           // data length = 8 bit
    outCommand(0x08);           // display and cursor off
    outCommand(0x06);           // cursor advance mode
    outCommand(0x01);           // clear display
    outCommand(0x02);           // cursor home
    outCommand(0x0E);           // display and cursor on
}

void Lcd::tstDsp()
{
    register unsigned char s;
    DDRA = 0;
    PORTA = 0xff;
    do {
        sbi(PORTG, RW);
        cbi(PORTC, RS);         // R/W = 1 , RS = 0
        sbi(PORTC, EN);         // EN = 1
        asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
        asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
        s = PINA;;
        cbi(PORTC, EN);         // EN = 0
    }
    while ((s & 0x80) && (s != 0xff));
}

void Lcd::setInterface()
{
    DDRA = 0xff;
    cbi(PORTG, RW);
    cbi(PORTC, RS);             // R/W = 0 , RS = 0
    sbi(PORTC, EN);             // EN = 1
    PORTA = 0x38;               // data
    asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
    asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
    cbi(PORTB, EN);
}

void Lcd::outCommand(unsigned char cmd)
{
    tstDsp();
    DDRA = 0xff;
    cbi(PORTG, RW);
    cbi(PORTC, RS);             // R/W = 0 , RS = 0
    sbi(PORTC, EN);             // EN = 1
    PORTA = cmd;                // data
    asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
    asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
    cbi(PORTC, EN);
}

void Lcd::write(unsigned char c)
{
    tstDsp();
    DDRA = 0xff;
    cbi(PORTG, RW);
    sbi(PORTC, RS);             // R/W = 0 , RS = 1
    sbi(PORTC, EN);             // EN = 1
    PORTA = c;                  // data
    asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
    asm("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
    cbi(PORTC, EN);
}

void Lcd::goTo(unsigned char y, unsigned char x)
{
    unsigned char pos = x;
    switch (y) {
        case 0:
            break;
        case 1:
            pos += 0x40;
            break;
        case 2:
            pos += 16;
            break;
        case 3:
            pos += 0x40;
            pos += 16;
            break;
    }
    pos |= 0x80;
    outCommand(pos);
}

void Lcd::print(unsigned char y, unsigned char x, char const *fmt, ...)
{
    goTo(y, x);
    va_list va;
    va_start(va, fmt);
    vfprintf_P(console, fmt, va);
}

void Lcd::clear()
{
    outCommand(1);              // Clear dispaly
    outCommand(2);              // cursor home
}

//eof
