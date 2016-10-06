#ifndef ADC_H
#define ADC_H
//
// C++ Interface: Adc
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class Adc{
public:
    Adc();
    static inline unsigned int getRms(unsigned char phase)
    {
        return (rms[phase]>>4);
    };
    static void  scan();
    static void adcInt();
    static Adc adc;
private:
    static volatile unsigned int val[3];
    static unsigned char ms;
    static volatile unsigned int rms[3];        //adc values *10
};

#endif
