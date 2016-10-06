#ifndef KBD_H
#define KBD_H
//
// C++ Interface: Kbd
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define BACK 0x04
#define FORW 0x10
#define UP 0x08
#define DOWN 0x40
#define ESC 0x20
#define ENT 0x80

class Kbd{
public:
    Kbd();
    static Kbd kbd;
    static void scan();
    static unsigned char get();
    static unsigned char tick;
    static unsigned char autoRepeat;
private:
    static unsigned char wp,rp,cnt,masq;
    static unsigned char buff[8];
    static unsigned char autoCount;
};

extern "C" int _getChar();        // used for stdin
#endif
