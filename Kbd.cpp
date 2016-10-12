//
// C++ Implementation: Kbd
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
#include <string.h>
#include <stdio.h>
}
#include "Kbd.h"
#include "Defines.h"
#include "Lcd.h"

//
// Static Class instances and members
//

Kbd Kbd::kbd;
unsigned char Kbd::wp, Kbd::rp, Kbd::cnt, Kbd::masq=0xff;
unsigned char  Kbd::autoRepeat, Kbd::autoCount, Kbd::tick=KBDTICK;
unsigned char Kbd::buff[8];


extern "C" {
int _getChar() {
    int c;

	    if (c=(int)Kbd::get()) return c;
	    else return EOF;

}
}

Kbd::Kbd()
{
    PORTE=0xFC;                // keyboard pullups
    DDRE&=~0xFC;               // and input
}

void Kbd::scan()
{
    if (tick) {
        tick--;
        return;
    }
    unsigned char k;
    unsigned char p=PINE|3;
    if (p==0xff) autoCount=0;
    if (autoRepeat&(p!=0xff)) {
        if (autoCount<10) autoCount++;
        if (autoCount>9)k=(~p)&(masq|0x48);
        else k=(~p)&masq;
    }
    else
        k=(~p)&masq;
    if (k&&(cnt<8)) {
        buff[wp]=k;
        cnt++;
        if (wp<7) wp++;
        else wp=0;
    }
    masq=p;
    tick=KBDTICK;
}

unsigned char Kbd::get()
{

    if (!cnt) return 0;
    unsigned char key=buff[rp];
    if (rp<7) rp++;
    else rp=0;
    cnt--;
    return key;
}
//eof
