//
// C++ Implementation: Nvr
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
#include <avr/eeprom.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/wdt.h>
}
#include "Defines.h"
#include "Dmx.h"
#include "Nvr.h"

//
// Static class instances and members
//

Nvr Nvr::nvr;

Setup Nvr::setup;
Mem Nvr::memNext;

Setup Nvr::setupEE EEPROM;
Mem Nvr::memEE[24] EEPROM;

void Nvr::initFirstTime()
{
    setup.signature = SIGNATURE;
    setup.dmxAddr = 1;
    setup.vref[0] = setup.vref[1] = setup.vref[2] = 230;
    setup.ref[0] = setup.ref[1] = setup.ref[2] = 470;
    setup.refTop[0] = setup.refTop[1] = setup.refTop[2] = 470;
    setup.refBot[0] = setup.refBot[1] = setup.refBot[2] = 350;
	setup.dpllEnable[0]=setup.dpllEnable[1]=setup.dpllEnable[2]=0x00; //DPLL initially disabled
    memset(&setup.outFunc, 0, 24);
    eeprom_write_block((unsigned char *) &setup, &setupEE, sizeof(Setup));
    for (int i = 0; i < MAXMEM; ++i) {
        eeprom_write_byte(&memEE[i].valid, 0);  // invalidate all memories
    }
}

Nvr::Nvr()
{
    unsigned int signature = eeprom_read_word(&setupEE.signature);
    if (signature != SIGNATURE)
        initFirstTime();
    else {
        // read the setup
        eeprom_read_block((unsigned char *) &setup, &setupEE,
                          sizeof(Setup));
        // and read the first mem into memNext
       memNext.valid = 0;
        for (int i = 0; i < MAXMEM; ++i) {
            memValid[i] = eeprom_read_byte(&memEE[i].valid);
            if (memValid[i] && (numNext<0)) 
            	numNext=i;
        }
    }
    numNext = numCurrent = numPrevious = -1;
 }

void Nvr::writeSetup()
{
    if (setup.dmxAddr > (512 - NUMCHAN))
        setup.dmxAddr = 512 - NUMCHAN;
    if (setup.dmxAddr < 1)
        setup.dmxAddr = 1;
    eeprom_write_block((unsigned char *) &setup, &setupEE, sizeof(Setup));
}

void Nvr::restoreSetup()
{
    eeprom_read_block((unsigned char *) &setup, &setupEE, sizeof(Setup));
}

/*
 * Memory go and goTo routines:
 *  They read the memory into the memNext and numNext
 */

bool Nvr::memGo()               // go
{
    numPrevious=numCurrent;
    numCurrent=numNext;
    int j;
    // if next mem exists return true
    if (numNext>=0) {
        eeprom_read_block((unsigned char *) &memNext,
                          (unsigned char *) &memEE[(int)numNext], sizeof(Mem));
        if ((numNext+1)<MAXMEM) {
        	for (j = numNext+1; j<MAXMEM; j++) {
        	   if (memValid[j]) {
        	   		numNext=j;
        	   		break;
        	   }
        	}
        	if (j>=MAXMEM) numNext=-1;
        }
        else numNext = -1;
        return true;
    }
    // else return blank
    else {
	    memNext.valid = 1;
	    memNext.timeIn = DEFAULT_TIMEIN;
	    memset(&memNext.chan[0], 0, 24);
    	for (j = 0; j<MAXMEM; j++) {
    	   if (memValid[j]) {
    	   		numNext=j;
    	   		break;
    	   }
    	}
    	if (j>=MAXMEM) numNext=-1;
	    return false;
    }
}

bool Nvr::memGoTo(char where)   // go to
{
	int j;
	if (where<0) {
	    memNext.valid = 1;
	    memNext.timeIn = DEFAULT_TIMEIN;
	    memset(&memNext.chan[0], 0, 24);
        numPrevious=numCurrent;
        numCurrent=where;
        numNext=-1;
    	for (j =  0; j<MAXMEM; j++) {
    	   if (memValid[j]) {
    	   		numNext=j;
    	   		break;
    	   }
    	}
    	return true;
	}
    if (memValid[(unsigned char) where]) {
        eeprom_read_block((unsigned char *) &memNext,
                          (unsigned char *) &memEE[(unsigned char) where],
                          sizeof(Mem));
        if (where<(MAXMEM-1)) {
	        numNext=-1;
	    	for (j = where+1; j<MAXMEM; j++) {
	    	   if (memValid[j]) {
	    	   		numNext=j;
	    	   		break;
	    	   }
	    	}
        }
        else numNext=-1;
        numPrevious=numCurrent;
        numCurrent=where;
        return true;
    } else
        return false;
}

bool Nvr::memBack()
{
    int i;
    numNext=numCurrent;
    numCurrent=numPrevious;    	
    if (numPrevious<0) {
	    memNext.valid = 1;
	    memNext.timeIn = DEFAULT_TIMEIN;
	    memset(&memNext.chan[0], 0, 24);
    }
    else {
        eeprom_read_block((unsigned char *) &memNext,
                          (unsigned char *) &memEE[(unsigned char) numPrevious],
                          sizeof(Mem));
    }
    if (numPrevious>0) {
		for (i = numPrevious-1; i>=0; i--) {
		   if (memValid[i]) {
		   		numPrevious=i;
		   		break;
		   }
		}
		if (i<0) numPrevious=-1;
    }
	else numPrevious=-1;
    return true;
}

void Nvr::memCancelGo()
{
    numNext=numCurrent;
    numCurrent=numPrevious;
}

/*
 * Memory store and storeTo routines:
 */

int Nvr::memStore()
{
    int i;
    for (i=MAXMEM-1; i>=0; i--)
        if (memValid[i]) {
           i++;
           break;
        }
    if (i>=MAXMEM) return -1;
    if (i<0) i=0;
    if (memValid[i]) return -1;
    memValid[i]=1;
    eeprom_write_byte(&memEE[i].valid,1);
    eeprom_write_word(&memEE[i].timeIn,DEFAULT_TIMEIN);
    eeprom_write_block((const void *)&Dmx::inVal[0], &memEE[i].chan[0],24);
    if (numNext<0) numNext=i;
    return i;
}

void Nvr::memStoreTo(unsigned char where)
{
    if (where>=MAXMEM) return;
    memValid[where]=1;
    eeprom_write_byte(&memEE[where].valid,1);
    eeprom_write_word(&memEE[where].timeIn,DEFAULT_TIMEIN);
    eeprom_write_block((const void *)&Dmx::inVal[0], &memEE[where].chan[0],24);
    if (numNext<0) numNext=where;
}

/*
 * Memory editing functions:
 */

unsigned int Nvr::memGetTime(unsigned char m)
{
    if (m>=MAXMEM) return 0xffff;
    if (!eeprom_read_byte(&memEE[m].valid)) return 0xffff;
    return eeprom_read_word(&memEE[m].timeIn);
}

bool Nvr::memSetTime(unsigned char m, unsigned int time)
{
    if (m>=MAXMEM) return false;
    if (!eeprom_read_byte(&memEE[m].valid)) return false;
    eeprom_write_word(&memEE[m].timeIn,time);
    return true;
}

bool Nvr::memDelete(unsigned char m)
{
    if (m>=MAXMEM) return false;
    if (!eeprom_read_byte(&memEE[m].valid)) return false;
    eeprom_write_byte(&memEE[m].valid,0);
    memValid[m]=0;
    if (numNext==m) {
        unsigned char j;
        numNext=-1;
        for (j = (m<(MAXMEM-1)) ? m+1 : MAXMEM-1; j<MAXMEM; j++) {
            if (memValid[j]) {
                  numNext=j;
                 break;
            }
        }
    }
    return true;
}

void Nvr::memClear()
{
    memset(&memValid[0],0,MAXMEM);
    for (int i = 0; i < MAXMEM; ++i) {
        eeprom_write_byte(&memEE[i].valid, 0);  // invalidate all memories
    }
    numNext=numCurrent=numPrevious=-1;
}
//eof
