//
// C++ Implementation: App
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
#include <avr/wdt.h>
#include <avr/interrupt.h>
}
#include "Defines.h"
#include "Lcd.h"
#include "Kbd.h"
#include "Scan.h"
#include "Dmx.h"
#include "Nvr.h"
#include "Adc.h"
#include "Menue.h"
#include "App.h"

#include "Spi.h"
#include "Curve.h"


//
// Static class instances and members
//

App App::app;

App::App()
{
}

//
// base screen output
//

void App::dmxScreen()           // output screen when DMX is active
{
   
    unsigned char rr, pg, page = 0, x , percent;
    int key;
    Lcd::lcd.clear();
    while (Dmx::valid) {
		wdt_reset();
        pg = page * 12;
        printfLcd(0, 0, "DMX ch. %3d-%3d:", Nvr::setup.dmxAddr + pg,
                  Nvr::setup.dmxAddr + 11 + pg);
        for (rr = 0; rr < 4; rr++) {
            x = rr * 3 + 5;
            percent = (unsigned char) (Dmx::inVal[rr + pg] * 100 / 255);
            Lcd::lcd.goTo(1, x);
            if (percent < 100)
                printfL("%2d ", percent);
            else
                printfL("FL ");
            Lcd::lcd.goTo(2, x);
            percent =(unsigned char) (Dmx::inVal[rr + 4 + pg] * 100 / 255);
            if (percent < 100)
                printfL("%2d ", percent);
            else
                printfL("FL ");
            percent = (unsigned char) (Dmx::inVal[rr + 8 + pg] * 100 / 255);
            Lcd::lcd.goTo(3, x);
            if (percent < 100)
                printfL("%2d ", percent);
            else
                printfL("FL ");
        }
        if ((key = getchar()) != EOF) {
            switch ((unsigned char) key) {
                case BACK:
                    page = 0;
                    break;
                case FORW:
                    if (NUMCHAN > 12)
                        page = 1;
                    break;
                case ENT:
                    Menue::menue.base();
                    break;
                default:
                    break;
            }
        }
    }
}

void App::localScreen()
{                               // output screen when DMX is inactive
	unsigned char rr, pr, x, percent;
    static unsigned char n;
	int key;
    Lcd::lcd.clear();
    Kbd::autoRepeat = 1;
  redisplay:
    wdt_reset();
    if (n < 8) {
        for (rr = 0; rr < 4; rr++) {
            x = rr * 4;
            percent = (unsigned char) (Dmx::inVal[rr] * 100 / 255);
            Lcd::lcd.goTo(0, x);
            if (n == rr)
                printfL(">%02d<", rr + 1);
            else
                printfL(" %02d ", rr + 1);
            Lcd::lcd.goTo(1, x);
            if (percent < 100){
                printfL(" %02d ", percent);
			}
            else
                printfL(" FL ");
            percent = (unsigned char) (Dmx::inVal[rr + 4] * 100 / 255);
            Lcd::lcd.goTo(2, x);
            if (n == rr + 4)
                printfL(">%02d<", rr + 4 + 1);
            else
                printfL(" %02d ", rr + 4 + 1);
            Lcd::lcd.goTo(3, x);
            if (percent < 100)
			   printfL(" %02d ", percent);
	
            else
                printfL(" FL ");
        }
        x = (n % 4) * 4 + 3;
        if (n < 4)
            Lcd::lcd.goTo(1, x);
        else
            Lcd::lcd.goTo(3, x);
    } else if (n < 16) {
#if NUMCHAN==24
        for (rr = 0; rr < 4; rr++) {
#else
        for (rr = 0; rr < 2; rr++) {
#endif
            x = rr * 4;
            percent = (unsigned char) (Dmx::inVal[rr + 8] * 100 / 255);
            Lcd::lcd.goTo(0, x);
            if (n == (rr + 8))
                printfL(">%02d<", rr + 8 + 1);
            else
                printfL(" %02d ", rr + 8 + 1);
            Lcd::lcd.goTo(1, x);
            if (percent < 100)
                printfL(" %02d ", percent);
            else
                printfL(" FL ");
            percent = (unsigned char) (Dmx::inVal[rr + 8 + 4] * 100 / 255);
            Lcd::lcd.goTo(2, x);
            if (n == (rr + 12))
                printfL(">%02d<", rr + 8 + 4 + 1);
            else
                printfL(" %02d ", rr + 8 + 4 + 1);
            Lcd::lcd.goTo(3, x);
            if (percent < 100)
                printfL(" %02d ", percent);
            else
                printfL(" FL ");
        }
        x = (n % 4) * 4 + 3;
        if (n < 12)
            Lcd::lcd.goTo(1, x);
        else
            Lcd::lcd.goTo(3, x);
    } else {
        for (rr = 0; rr < 4; rr++) {
            x = rr * 4;
            percent = (unsigned char) (Dmx::inVal[rr + 16] * 100 / 255);
            Lcd::lcd.goTo(0, x);
            if (n == (rr + 16))
                printfL(">%02d<", rr + 16 + 1);
            else
                printfL(" %02d ", rr + 16 + 1);
            Lcd::lcd.goTo(1, x);
            if (percent < 100)
                printfL(" %02d ", percent);
            else
                printfL(" FL ");
            percent =
                (unsigned char) (Dmx::inVal[rr + 16 + 4] * 100 / 255);
            Lcd::lcd.goTo(2, x);
            if (n == (rr + 16 + 4))
                printfL(">%02d<", rr + 16 + 4 + 1);
            else
                printfL(" %02d ", rr + 16 + 4 + 1);
            Lcd::lcd.goTo(3, x);
            if (percent < 100)
                printfL(" %02d ", percent);
            else
                printfL(" FL ");
        }
        x = (n % 4) * 4 + 3;
        if (n < 20)
            Lcd::lcd.goTo(1, x);
        else
            Lcd::lcd.goTo(3, x);
    }
  readkey:

   while ( (key = _getChar()) == EOF) {
		wdt_reset();
        if (Dmx::valid) {
            Kbd::autoRepeat = 0;
            return;
        }
    }
    switch ((unsigned char) key) {
        case BACK:
            if (n)
                n--;
            goto redisplay;
        case FORW:
            if (n < (NUMCHAN - 1))
                n++;
            goto redisplay;
        case UP:
            percent = (unsigned char) (Dmx::inVal[n] * 100 / 255);
            do {
                if (Dmx::inVal[n] >= 255)
                    break;
                Dmx::inVal[n]++;

                pr = (unsigned char) (Dmx::inVal[n] * 100 / 255);
            }
            while (pr == percent);
            goto redisplay;
        case DOWN:
            percent = (unsigned char) (Dmx::inVal[n] * 100 / 255);
            do {
                if (!Dmx::inVal[n])
                    break;
                Dmx::inVal[n]--;

                pr = (unsigned char) (Dmx::inVal[n] * 100 / 255);
            }
            while (pr == percent);
            goto redisplay;
        case ENT:
            Kbd::autoRepeat = 0;
            Menue::menue.base();
            Kbd::autoRepeat = 1;
            break;

        default:
            goto readkey;
    }
}


//
// aplication main loop
//

void App::run()
{
    printfLcd(0, 0, "Elsis digital ++");
    printfLcd(1, 0, "    V  %d.%d        ", VERSION/10, VERSION%10);
    printfLcd(3, 0, " ...booting...  ");

	for (unsigned long wt = 0; wt < 100000 /*1000000*/; wt++) {
        wdt_reset();
        asm("nop\n");
    }
    sei();                      // get the world going
    for (;;) {
       if (Dmx::valid )
            dmxScreen();
        else
            localScreen();
        wdt_reset();
    }
}

int main()
{
	wdt_enable(WDTO_1S);      // watchdog to 1 second
    App::app.run();
    return 0;
}
