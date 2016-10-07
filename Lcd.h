#ifndef LCD_H
#define LCD_H
//
// C++ Interface: Lcd
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
#include <avr/pgmspace.h>
#include <stdio.h>
// use a c++ compatible PSTR here
#define _PSTR(s) ({static char __c[] PROGMEM = (s); &__c[0];})
#define printfL(fmt, args...)   printf_P( PSTR(fmt), ## args)
}

class Lcd{
public:
    Lcd();
    static Lcd lcd;
    void clear();
    static void write(unsigned char c);
    void goTo(unsigned char y, unsigned char x);
    void print(unsigned char y, unsigned char x, char const *fmt, ...);
	void outCommand(unsigned char cmd);
	void setInterface();
	static void tstDsp();
private:
   // static void tstDsp();
  //  void setInterface();
   // void outCommand(unsigned char cmd);
    static FILE * console;
};

#define printfLcd(x,y,fmt,args...)  Lcd::lcd.print(x,y, PSTR(fmt) , ## args)
#endif
