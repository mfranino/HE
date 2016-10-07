#ifndef SPI_H
#define SPI_H
//
// C++ Interface: Spi
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

typedef struct _longbyte {
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
}
longbyte;

typedef union _lword {
    unsigned long l;
    longbyte b;
}
lword;

class Spi{
public:
    Spi();
    static void scan();
    static Spi spi;
	static inline unsigned char getPhasePtr(unsigned char chn)
    {
        return pgm_read_byte_near((unsigned int)&phasePtr[chn]);
    };
	static inline unsigned char getPhaseTab(unsigned char chn)
    {
        return  pgm_read_byte_near( ( unsigned int )&phaseTab[chn]);
    };
    static unsigned char outVal[24]; // output values 
    static volatile lword outDirekt;
    static volatile unsigned char outputCurrent[24], outputVoltage[24]; //diagnostic
    static volatile unsigned char outputStatus[3];    // diagnostic
    //static unsigned char dpllEnable[3]; //!!!
private:
    static unsigned char cnt,flagCurVol;
    static unsigned char phaseTab[24], phasePtr[24];
};
#endif
