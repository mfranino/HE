#ifndef DDA_H
#define DDA_H
//
// Dmx:: - Dda.h
//
// Description: 
//   Direct Diferential analyzer
//
// Author: Iztok Zupet <iz@elsis.si>, Sep 19, 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class Dda{
public:
   Dda();
   static void init(int x1,int x2,int y1,int y2);
   static int next();
   static Dda dda;
private:
    static int dx,dy,r;
    static int xv,yv;
    static int x,y;
    static int x0,y0,xx,yy;
};
#endif
