//
// Dmx: Dda.cpp
//
// Description: 
//  Direct diferential analyzer
//
// Author: Iztok Zupet <iz@elsis.si>, Sep 19, 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
extern "C" {
#include "stdlib.h"
}
#include "Defines.h"
#include "Dda.h"

//
// static class instance and members Dda Dda::dda;
//

int Dda::dx, Dda::dy, Dda::r;
int Dda::xv, Dda::yv;
int Dda::x, Dda::y;
int Dda::x0, Dda::y0, Dda::xx, Dda::yy;

Dda::Dda()
{
}

void Dda::init(int x1, int x2, int y1, int y2)
{
    dx = abs(x2 - x1);
    xv = (x2 > x1) ? 1 : -1;
    dy = abs(y2 - y1);
    yv = (y2 > y1) ? 1 : -1;
    x = x0 = x1;
    y = y0 = y1;
    xx = x2;
    yy = y2;
    r = 0;
}

int Dda::next()
{                               /* 20th century math... */
    if (dx >= dy) {
        x += xv;
        r += dy;
        if (r < dx)
            return y;
        y += yv;
        r -= dx;
    } else {
      loop:
        y += yv;
        r += dx;
        if (r < dy)
            goto loop;
        x += xv;
        r -= dy;
    }
    return y;
}
