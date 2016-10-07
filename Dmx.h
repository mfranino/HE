#ifndef DMX_H
#define DMX_H
//
// C++ Interface: Dmx
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class Dmx{
public:
    Dmx();
    static void dmx1Int();
    static void dmx2Int();
    static void merge();
    static Dmx dmx;
    static volatile unsigned char inVal[24]; // inVal[24] -> chan values after dmx merge// rmsVal[24] -> chan values after phisiological curve // outVal[24] -> chan values after the RMS curve
    static unsigned char mode  ;    // 0=1only, 1=2 only, 2=merge both
    static volatile unsigned char valid;
private:
    static volatile unsigned char first1,scan1,read1, flag1;
    static volatile unsigned char first2,scan2,read2,flag2;
    static volatile unsigned int cnt1,cnt2;
    static volatile unsigned char * ptr1, * ptr2;
    static volatile unsigned char valid1, valid2;                  // dmx valid flags
    static volatile unsigned char inBuff1[24], inBuff2[24];        // dmx input buffers
};
#endif
