#ifndef SCAN_H
#define SCAN_H
//
// C++ Interface: Scan
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class Scan{
public:
    Scan();
    void xfadeGo();                      // start xfade
    void xfadeCancelGo();
    static void scanAll();
    static Scan scan;
    static unsigned int tcnt1Lap;    // used for timer diagnostic must be <1500
    volatile static unsigned char xfadeFlag;
    static unsigned char xfadeHold;
    volatile static unsigned int xfadeVal;
	
	
private:
    static void scanOutputs();
    static void xfade();
    static unsigned char max(unsigned char a, unsigned char b);
    static unsigned char cnt;
    static unsigned char chnVal[24],rmsVal[24]; // inVal[24] -> chan values after dmx merge// rmsVal[24] -> chan values after phisiological curve // outVal[24] -> chan values after the RMS curve
    static unsigned int time;
    static unsigned char xfadeBack;
};
#endif
