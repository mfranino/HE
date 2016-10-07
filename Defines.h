#ifndef DEFINES_H
#define DEFINES_H
// Common defines
//
// Description: 
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define VERSION 10

#define MEGACLOCK   16          // can be 4,8,12,16
#define SIGNATURE 0xAA55        // eeprom signature

#define NUMCHAN 12//24
#define KBDTICK 66 // 66              // number of ticks for keyboard scan  
#define REFRESHTICK 33          // number of ticks for channel output refresh

#define VTOP 230                // max output voltage
#define VBOT 180                // min required voltage for stabilization

#define MAXMEM 24               // number of memories
#define DEFAULT_TIMEIN 90       // number of ticks for timein
#endif
