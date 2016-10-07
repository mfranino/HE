#ifndef MENUE_H
#define MENUE_H
//
// C++ Interface: Menue
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class Menue{
public:
    void base();
    static Menue menue;
	static int tick;
private:
    void mem();
    int memStoreTo();
    void memEdit();
    void memNonExistent(int m);
    void play();
    void playXfade();
    void setup();
    void setupVref();
    void setupDmx();
    void setupCurve();
    unsigned int doEdit(unsigned int &param, unsigned char x, \
                        unsigned char n, unsigned int max, unsigned int min);
	int memSelect();
    void voltageScreen();
    void setupTimeBase();
    void delay();
    unsigned char getch();
};
#endif
