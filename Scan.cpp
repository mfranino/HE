//
// C++ Implementation: Scan
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
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <string.h>
}
#include "Defines.h"
#include "Adc.h"
#include "Kbd.h"
#include "Spi.h"
#include "Dmx.h"
#include "Nvr.h"
#include "Curve.h"
#include "Dda.h"
#include "Scan.h"

//
// static classs instance and members
//

Scan Scan::scan;
unsigned char Scan::cnt=1;
unsigned char Scan::chnVal[24];
unsigned char Scan::rmsVal[24];
unsigned int Scan::tcnt1Lap;
volatile unsigned char Scan::xfadeFlag;
unsigned char  Scan::xfadeHold, Scan::xfadeBack;
volatile unsigned int Scan::xfadeVal;
unsigned int Scan::time;

Scan::Scan()
{
    TCCR1A = 0;
    OCR1A = 2000u; // time costant (16Mhz/8/2000 = 1KHz)
    TCNT1 = 0;
    TCCR1B = 0x0a; // clear on OC1A, 16 prescaler
    sbi( TIMSK, OCIE1A ); // enable its interrupt
}

//
// Xfade
//

unsigned char Scan::max(unsigned char a, unsigned char b)
{
    if (a>b) return a;
    else return b;
}


#define high(x) \
 ({unsigned char t;  asm volatile ("lds %0,  (%1+1)" :"=r"(t) :"m"(x)); t;})

void Scan::xfade()
{
    static unsigned int ma,mb;
    if (!xfadeFlag) {
        memcpy(&chnVal[0],(const void *)&Dmx::inVal[0],24);
        return;
    }
    if (xfadeHold) return;
    time++;
    xfadeVal=Dda::next();
    unsigned char vmax;
    unsigned int v;
    for (unsigned char chn=0; chn<24; chn++) {    
        ma = Dmx::inVal[chn] * (256-xfadeVal);
        mb = Nvr::memNext.chan[chn] * xfadeVal;
        v = high(ma) + high(mb);
        vmax=max(Dmx::inVal[chn], Nvr::memNext.chan[chn]);
        if (v>vmax) v=vmax;
        chnVal[chn]=v;
    }
    if (!xfadeBack) {
	    if (xfadeVal>=256) {
	        memcpy((void*)&Dmx::inVal[0],(const void *)&Nvr::memNext.chan[0],24);
	        xfadeFlag=0;
	    }
    }
    else {
    	if (!xfadeVal)
        	xfadeFlag=0;
    }
}

void Scan::xfadeGo()
{
    Dda::init(0,Nvr::memNext.timeIn,0,256);
    time=0;
    xfadeHold=0;
    xfadeVal=0;
    xfadeBack=0;
    xfadeFlag=1;
}

void Scan::xfadeCancelGo()
{
    cbi( TIMSK, OCIE1A );               // disable timer interrupt
    Dda::init(time,0,xfadeVal,0);       // revert dda
    xfadeBack=1;
    sbi( TIMSK, OCIE1A );               // reenable timer interrupt
}


void Scan::scanOutputs()
{
    unsigned char phase;
    unsigned int rmsval;
    unsigned long dir = 0;
    Spi::dpllEnable[0]=Spi::dpllEnable[1]=Spi::dpllEnable[2]=0x10;
    unsigned int curRms;
    for (unsigned char chn=0; chn<24; chn++) {
    	rmsVal[chn]=Curve::getRms(chn,chnVal[chn]);
        // figure out the phase
        phase=Spi::getPhasePtr(chn);
        curRms=Adc::getRms(phase);
        // calculate stable RMS value
        // if fluo then no stabilization
        if ( (Nvr::setup.outFunc[chn] == FLUODPLL) ||
             (Nvr::setup.outFunc[chn] == DIREKT) ||
             (curRms<Nvr::setup.refBot[phase] ) )
            rmsval=rmsVal[chn];
        else if (Nvr::setup.outFunc[chn]==FLUOPHASE) {
            rmsval=rmsVal[chn];
            Spi::dpllEnable[phase]&=0;
        }
        else
            rmsval=(unsigned int)(((unsigned long)rmsVal[chn]*
                    Nvr::setup.refTop[phase])/curRms);
        if (rmsval>255) rmsval=255;
        // get the phase angle value
        Spi::outVal[chn]=Curve::getPhaseAngle(rmsval);
        // figure out which channel is direkt
        if ( Nvr::setup.outFunc[chn] == 4 )
            dir |= 1 << Spi::getPhaseTab(chn);
    }
    Spi::outDirekt.l = dir;
}

void Scan::scanAll()
{
    unsigned int tcnt1Start=TCNT1;
    Spi::scan();
    Adc::scan();
    Kbd::scan();
    if (cnt==REFRESHTICK-2)
        Dmx::merge();
    else if ((cnt==REFRESHTICK-1))
        xfade();
    if(++cnt>REFRESHTICK) {
        scanOutputs();
        cnt=0;
        unsigned int tcnt1Stop=TCNT1;
        tcnt1Lap=tcnt1Stop-tcnt1Start;
    }
}

extern "C"
INTERRUPT( SIG_OUTPUT_COMPARE1A )
{
    cbi( TIMSK, OCIE1A ); // disable own interrupt
    Scan::scanAll();
    cli();
    sbi( TIMSK, OCIE1A ); // reenable own interrupt
}
//eof
