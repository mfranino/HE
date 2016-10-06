//
// C++ Implementation: Menue
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
}
#include "Defines.h"
#include "Lcd.h"
#include "Kbd.h"
#include "Nvr.h"
#include "Adc.h"
#include "Dmx.h"
#include "Curve.h"
#include "Scan.h"
#include "Menue.h"

/*!
    Definition of static class instance and members
*/ 
 
Menue Menue::menue;

//
// mem
//

void Menue::base()
{
    unsigned char key;
  repaint:
    wdt_reset();
    Lcd::lcd.clear();
    printfLcd(0, 0, "  Main          ");
    printfLcd(1, 0, "       Menu     ");
    printfLcd(3, 0, "Mem   Play   Stp");
    for (;;) {
        switch (key=getch()) {
            case ENT:
                return;
            case ESC:
                return;
            case BACK:
                mem();
                return;
            case UP:
                if (Dmx::valid) break;
                else {
                    play();
                    return;
                }
            case FORW:
                setup();
                goto repaint;
        }
        wdt_reset();
    }
}

//
// mem
//

void Menue::mem()
{
    unsigned char key;
    int sel;
  repaint:
    wdt_reset();
    Lcd::lcd.clear();
    printfLcd(0, 0, "    Memory      ");
    printfLcd(1, 0, "  Current %d     ", Nvr::nvr.numCurrent+1);
    printfLcd(3, 0, "Edit  Sto   Sto+");
    for (;;) {
        switch (key=getch()) {
            case ENT:
                break;
            case ESC:
                return;
            case BACK:
				memEdit();
                goto repaint;
            case UP:
                if ((sel=memStoreTo())>(-1)) {
                    Nvr::nvr.memStoreTo((unsigned char) sel);
    				printfLcd(0, 0, "  Stored to:    ");
    				printfLcd(1, 0, "          %d     ",sel+1);
    				delay();
                    return;
                } 
                else {
    				printfLcd(0, 0, "    Escaped     ");
    				printfLcd(1, 0, "                ");
    				delay();
    				goto repaint;
                }
            case FORW:
                if ((sel = Nvr::nvr.memStore()) > (-1)) {
    				printfLcd(0, 0, "  Stored to:    ");
    				printfLcd(1, 0, "          %d     ",sel+1);
    				delay();
                    return;
                }
                else {
    				printfLcd(0, 0, "    Memory      ");
    				printfLcd(1, 0, "       full     ");
    				delay();
    				goto repaint;
                }
        }
        wdt_reset();
    }
}

int Menue::memStoreTo()
{
    unsigned char key;
    Lcd::lcd.clear();
    printfLcd(0, 0, "     Memory     ");
    int sel=Nvr::nvr.numCurrent+1;
    printfLcd(1, 0, "   Current:  %2d  ", sel);
    if (!sel) sel=1;
    Kbd::autoRepeat = 1;
  repaint:
    wdt_reset();
    printfLcd(3, 0, "   Store to: %2d  ", sel);
    for (;;) {
        switch (key=getch()) {
            case ENT:
			    Kbd::autoRepeat = 0;
                return sel-1;
            case ESC:
			    Kbd::autoRepeat = 0;
                return -1;
            case UP:
            case FORW:
                if (sel<MAXMEM) sel++;
                goto repaint;
            case DOWN:
            case BACK:
                if (sel>1) sel--;
                goto repaint;
        }
        wdt_reset();
    }
}

void Menue::memEdit()
{
    unsigned char key;
    int sel;
    unsigned int time1;
    static unsigned int timeIn;
  repaint:
    wdt_reset();
    Lcd::lcd.clear();
    printfLcd(0, 0, "    Memory      ");
    printfLcd(1, 0, "     Edit       ");
    printfLcd(3, 0, "Time   Del   Clr");
    for (;;) {
        switch (key=getch()) {
        	case BACK:
			    printfLcd(0, 0, " Select memory  ");
			    printfLcd(1, 0, " to edit time   ");
                if ((sel=memSelect())>=0) {
                    if (!Nvr::nvr.memValid[sel])
                        memNonExistent(sel);
                    else {
                        Lcd::lcd.clear();
                        printfLcd(0, 0, "  Specify new   ");
                        printfLcd(1, 0, "     time       ");
                        int time=Nvr::nvr.memGetTime(sel);
                        timeIn=(unsigned int)((unsigned long)time*10*REFRESHTICK/1000);
                        if ((time1=doEdit(timeIn,6,4,9999,1))!=timeIn) {
                            timeIn=(unsigned int)((unsigned long)time1*1000/(10*REFRESHTICK));
                            Nvr::nvr.memSetTime(sel,timeIn);
                        }
                    }
                }
        		goto repaint;
        	case UP:
			    printfLcd(0, 0, " Select memory  ");
			    printfLcd(1, 0, "   to delete    ");
                if ((sel=memSelect())>=0) {
                    if (!Nvr::nvr.memValid[sel])
                        memNonExistent(sel);
                    else {
                        Lcd::lcd.clear();
                        printfLcd(0, 0, "  Press Enter   ");
                        printfLcd(1, 0, " to delete m%d   ",sel+1);
                        printfLcd(2, 0, " or press Escape");
                        printfLcd(3, 0, "   to cancel    ");
                        unsigned char k;
                        do k=getch();
                        while ((k!=ENT) && (k!=ESC));
                        if (k==ENT)
                            Nvr::nvr.memDelete(sel);
                    }
                }
        		goto repaint;
        	case FORW:
                Lcd::lcd.clear();
                printfLcd(0, 0, " Press Enter to ");
                printfLcd(1, 0, " clear memmories");
                printfLcd(2, 0, " or press Escape");
                printfLcd(3, 0, "   to cancel    ");
                unsigned char k;
                do k=getch();
                while ((k!=ENT) && (k!=ESC));
                if (k==ENT) {
                    printfLcd(1, 0, "    proceed     ");
                    do k=getch();
                    while ((k!=ENT) && (k!=ESC));
                    if (k==ENT) {
                        Lcd::lcd.clear();
                        printfLcd(0, 0, "  Clearing all  ");
                        printfLcd(1, 0, "    memmories   ");
                        Nvr::nvr.memClear();
                        delay();
                    }
                }                
        		goto repaint;
            case ENT:
                return;
            case ESC:
                return;
        }
        wdt_reset();
    }
}

void Menue::memNonExistent(int m)
{
    Lcd::lcd.clear();
    printfLcd(0, 0, "   Memory  %2d   ",m);
    printfLcd(1, 0, "    does not    ");
    printfLcd(2, 0, "     exist!     ");
    delay();
}

//
// Playback
//

void Menue::play()
{
    unsigned char key;
    int sel;
  repaint:
    wdt_reset();
    Lcd::lcd.clear();
    printfLcd(0, 0, "   Playback     ");
    printfLcd(1, 0, "  Next    %d     ", Nvr::nvr.numNext+1);
    printfLcd(3, 0, "Back   To   Next");
    for (;;) {
        switch (key=getch()) {
            case ENT:
                break;
            case ESC:
                return;
            case BACK:
                if (Nvr::nvr.memBack()) {
                    Scan::scan.xfadeGo();
                    playXfade();
                    goto repaint;
                }
                else break;
            case UP:
            	printfLcd(1,0,"   Select next  ");
                if ((sel=memSelect())>=(-1)) {
                    Nvr::nvr.memGoTo(((char)sel));
                    Scan::scan.xfadeGo();
                    playXfade();
                }
                goto repaint;
            case FORW:
                Nvr::nvr.memGo();
                Scan::scan.xfadeGo();
                playXfade();
                goto repaint;
        }
        wdt_reset();
    }
}


void Menue::playXfade()
{
    char key;
    int p;
    Lcd::lcd.clear();
    printfLcd(0, 0, "  Crossfade     ");
    printfLcd(1, 0, "  percent =      ");
    printfLcd(3, 0, "Back  Hold   Go ");
    while (Scan::xfadeFlag) {
        wdt_reset();
        p=(Scan::xfadeVal*100)>>8;
        Lcd::lcd.goTo(1,12);
        if (p<100) printfL("%2d",p);
        else printfL("FL");
        if ((key=getchar())!=EOF) {
            switch ((unsigned char)key) {
                case UP:
                    Scan::xfadeHold=1;
                    break;
                case FORW:
                    Scan::xfadeHold=0;
                    break;
                case BACK:
                    Nvr::nvr.memCancelGo();
                    Scan::xfadeHold=0;
                    Scan::scan.xfadeCancelGo();
                    break;
            }
        }
    }
}



//
// Setup
//

void Menue::setup()
{
    unsigned char key;
  rep1:
    wdt_reset();
    Lcd::lcd.clear();
    printfLcd(0, 0, " Configuration  ");
    printfLcd(1, 0, "     Setup      ");
    printfLcd(3, 0, "Dmx   Curv  Diag");
    wdt_reset();
    for (;;) {
        switch (key=getch()) {
            case ENT:
                return;
            case ESC:
                return;
            case BACK:
                setupDmx();
                goto rep1;
            case UP:
                setupCurve();
                goto rep1;
            case FORW:
                voltageScreen();
                goto rep1;
            default:
                break;
        }
        wdt_reset();
    }
}

void Menue::setupDmx()
{
    unsigned int val;
    Lcd::lcd.clear();
    printfLcd(0, 0, " Configure      ");
    printfLcd(1, 0, "    DMX address ");
    wdt_reset();
    if ((val = doEdit(Nvr::setup.dmxAddr, 7,3,512-NUMCHAN,1)) \
             != Nvr::setup.dmxAddr) {
        Nvr::setup.dmxAddr = val;
        Nvr::nvr.writeSetup();
    }
}

void Menue::setupCurve()
{
    unsigned char rr, c, page, x;
    unsigned char key;
    page = 0;
    Lcd::lcd.clear();
    printfLcd(0, 0, "  Output curve  ");
    printfLcd(1, 0, "      type      ");
  rdsp:
    wdt_reset();
    for (rr = 0; rr < 4; rr++) {
        x = rr * 4;
        c = Nvr::setup.outFunc[rr + (page / 4) * 4];
        Lcd::lcd.goTo(2, x);
        if ((page % 4) == rr)
            printfL(">%02d<", rr + 1 + (page / 4) * 4);
        else
            printfL(" %02d ", rr + 1 + (page / 4) * 4);
        printfLcd(3, x, " %1d ", c + 1);
    }
    x = (page % 4) * 4 + 3;
    Lcd::lcd.goTo(3, x);
    wdt_reset();
    switch (key=getch()) {
        case BACK:
            if (page)
                page--;
            goto rdsp;
        case FORW:
            if (page < (NUMCHAN - 1))
                page++;
            goto rdsp;
        case UP:
            if (Nvr::setup.outFunc[page] < (MAXCURVE - 1))
                Nvr::setup.outFunc[page]++;
            goto rdsp;
        case DOWN:
            if (Nvr::setup.outFunc[page])
                Nvr::setup.outFunc[page]--;
            goto rdsp;
        case ENT:
            Nvr::nvr.writeSetup();
            break;
        case ESC:
            break;
    }
}

/*!
    Diagnstic of voltage measuring
 */
void Menue::voltageScreen()
{
    char key;
    unsigned char term = 0, state = 0;
    Lcd::lcd.clear();
    do {
        printfLcd(0, 0, "  Live voltage  ");
        printfLcd(1, 5, "L1=%3dV",(unsigned) ((unsigned long) Nvr::setup.vref[0]
                                        * Adc::getRms(0) / Nvr::setup.ref[0]));
        printfLcd(2, 5, "L2=%3dV",(unsigned) ((unsigned long) Nvr::setup.vref[1]
                                        * Adc::getRms(1) / Nvr::setup.ref[1]));
        printfLcd(3, 5, "L3=%3dV",(unsigned) ((unsigned long) Nvr::setup.vref[2]
                                        * Adc::getRms(2) / Nvr::setup.ref[2]));
        key = getchar();
        wdt_reset();
        if (key != EOF) {
            if (!state)
                switch ((unsigned char) key) {
                    case BACK:
                        state = 1;
                        break;
                    case ENT:
                    case ESC:
                        term = 1;
                        break;
            } else if (state == 1)
                switch ((unsigned char) key) {
                    case DOWN:
                        state = 2;
                        break;
                    case ENT:
                    case ESC:
                        term = 1;
                        break;
                    default:
                        state = 0;
                        break;
            } else
                switch ((unsigned char) key) {
                    case ENT:
                        setupVref();
                        Lcd::lcd.clear();
                        state = 0;
                        break;
                    case ESC:
                        term = 1;
                        break;
                    default:
                        state = 0;
                        break;
                }
        }
    } while (!term);
}

void Menue::setupVref()
{
    unsigned int val;
    unsigned char p = 0, n, x[3];
    unsigned char key;
    x[0] = 0;
    x[1] = 6;
    x[2] = 12;
    Lcd::lcd.clear();
    printfLcd(0, 0, " Reference      ");
    printfLcd(1, 0, "    voltages    ");
    do {
        wdt_reset();
        for (n = 0; n < 3; n++) {
            Lcd::lcd.goTo(2, x[n]);
            if (p == n)
                printfL(">L%d<", n + 1);
            else
                printfL(" L%d ", n + 1);
            printfLcd(3, x[n], "%3d", Nvr::setup.vref[n]);
        }
        switch (key=getch()) {
            case FORW:
                if (p < 2)
                    p++;
                break;
            case BACK:
                if (p)
                    p--;
                break;
            case DOWN:
                val = doEdit(Nvr::setup.vref[p], x[p],3,280,180);
                if (val != Nvr::setup.vref[p]) {
                    Nvr::setup.vref[p] = val;
                    Nvr::nvr.writeSetup();
                }
                break;
        }
    } while (((unsigned char) key != ESC) && ((unsigned char) key != ENT));
    if (key == ESC)
        return;
    // Voltage reference calculation
    for (n = 0; n < 3; n++) {
        Nvr::setup.ref[n] = Adc::getRms(n);
        Nvr::setup.refTop[n] =
            (unsigned) ((unsigned long) Nvr::setup.ref[n] * VTOP /
                        Nvr::setup.vref[n]);
        Nvr::setup.refBot[n] =
            (unsigned) ((unsigned long) Nvr::setup.ref[n] * VBOT /
                        Nvr::setup.vref[n]);
    }
    Nvr::nvr.writeSetup();
    wdt_reset();
}


/*!
    Basic number editing
 */
unsigned int Menue::doEdit(unsigned int &param, unsigned char x, \
                           unsigned char n, unsigned int max, unsigned int min)
{
    if (n>4) n=4;
    static unsigned char d[4];
    unsigned char term = 0, p = 0;
    unsigned int val = param;
    unsigned char key;
    for (unsigned char i = 0; i < 4; i++) {
        d[i] = val % 10;
        val = val / 10;
    }
    Kbd::autoRepeat = 1;
    do {
        val = d[0] + d[1] * 10 + d[2] * 100 + d[3] * 1000;
        if (val>max) {
        	val=max;
        	unsigned int val1=val;
		    for (unsigned char i = 0; i < 4; i++) {
		        d[i] = val1 % 10;
		        val1 = val1 / 10;
		    }
        }
        if (val<min) {
        	val=min;
        	unsigned int val2=val;
		    for (unsigned char i = 0; i < 4; i++) {
		        d[i] = val2 % 10;
		        val2 = val2 / 10;
		    }
        }
        wdt_reset();
        if (n<4)
            printfLcd(3, x, "%03d", val);
        else
            printfLcd(3, x, "%04d", val);
        Lcd::lcd.goTo(3, x + (n-1) - p);
        switch (key=getch()) {
            case FORW:
                if (p)
                    p--;
                break;
            case BACK:
                if (p < (n-1))
                    p++;
                break;
            case UP:
                if (d[p] < 9)
                    d[p]++;
                break;
            case DOWN:
                if (d[p])
                    d[p]--;
                break;
            case ESC:
                val = param;
            case ENT:
                term = 1;
                break;
        }
    } while (!term);
    Kbd::autoRepeat = 0;
    return val;
}

int Menue::memSelect()
{
    unsigned char key;
    int s,sel=Nvr::nvr.numCurrent+1;
    printfLcd(2, 0, "   Current: %2d   ", sel);
	Kbd::autoRepeat = 1;
	if (sel<1) {
		for (s=0; s<MAXMEM; s++)
			if (Nvr::nvr.memValid[s]) break;
		if (s<MAXMEM) sel=s+1;
	}
  repaint:
    wdt_reset();
    printfLcd(3, 0, "   Select : %2d   ", sel);
    for (;;) {
        switch (key=getch()) {
            case ENT:
			    Kbd::autoRepeat = 0;
                return sel-1;
            case ESC:
			    Kbd::autoRepeat = 0;
                return -2;
            case UP:
            case FORW:
                for (s = sel<MAXMEM ? sel : MAXMEM-1 ; s<(MAXMEM-1); s++)
                    if (Nvr::nvr.memValid[s]) break;
				sel = Nvr::nvr.memValid[s] ? s+1 : 0;
                goto repaint;
            case DOWN:
            case BACK:
            	if ((sel-2)>=0) {
	                for (s = sel-2; s >= 0; s--)
	                    if (Nvr::nvr.memValid[s]) break;
	                sel = s+1;
            	}
            	else if ((sel-2)==-1)
            		sel=0;
            	else {
	                for (s = MAXMEM-1; s >= 0; s--)
	                    if (Nvr::nvr.memValid[s]) break;
	                sel=s+1;
            	}
                goto repaint;
        }
        wdt_reset();
    }
}



void Menue::delay()
{
    for (unsigned long wt = 0; wt < 1000000; wt++) {
        wdt_reset();
        asm("nop\n");
    }
	
}

unsigned char Menue::getch()
{
    char key;
    while ((key=getchar()) == EOF)
        wdt_reset();
    return (unsigned char) key;
}
//eof
