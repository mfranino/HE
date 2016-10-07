#ifndef CURVE_H
#define CURVE_H
//
// C++ Interface: Curve
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define MAXCURVE 5              // number of curves
#define DIREKT 5
#define FLUODPLL 3
#define FLUOPHASE 4

class Curve{
public:
    Curve();
    static Curve curve;
	static unsigned char getRms(unsigned char chn, unsigned char val);
	static unsigned char getRmsP(unsigned char chn, unsigned char val);
    static unsigned char getPhaseAngle(unsigned char inVal);
	static unsigned char test(unsigned char inVal);
	
	
private:
    static const unsigned char cElsis[256];
    static const unsigned char cElsisPreheat[256];
    static const unsigned char cFluo[256];
    static const unsigned char cDirect[256];
    static PGM_P const outFuncTable[MAXCURVE];
	static const unsigned char cPower[256];    // RMS s krivulja
    static const unsigned char out[MAXCURVE][256];
};
#endif
