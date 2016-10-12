#ifndef NVR_H
#define NVR_H
//
// C++ Interface: Nvr
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define EEPROM __attribute__((section(".eeprom")))

typedef struct _Setup {
    unsigned int signature;
    unsigned int dmxAddr;               // dmx address
    unsigned int vref[3];               // phase voltage references
    unsigned int ref[3];
    unsigned int refTop[3], refBot[3];  // top and bottom voltage reference
    unsigned char outFunc[24];
	unsigned char dpllEnable[3]; //DPLL enable flag
} Setup;

typedef struct _Mem {
    unsigned char valid;
    unsigned int timeIn;
    unsigned char chan[24];
} Mem;

class Nvr{
public:
    Nvr();
    static Nvr nvr;
    void writeSetup();
    void restoreSetup();
    bool memGo();        // go to next 
    void memCancelGo();    // cancel go                            
    bool memGoTo(char);  // goto
    bool memBack();      // back
    int memStore();
    void memStoreTo(unsigned char where);
    unsigned int memGetTime(unsigned char mem);
    bool memSetTime(unsigned char mem, unsigned int time);
    bool memDelete(unsigned char mem);
    void memClear();
    static Setup setup;         // in memory setup
    static Mem memNext;         // next memory
    char numCurrent, numNext, numPrevious;     // [-1 = empty]
    unsigned char memValid[MAXMEM];
	
private:
    void initFirstTime();
    static Setup setupEE;              // eeprom setup
    static Mem memEE[MAXMEM];
};
#endif
