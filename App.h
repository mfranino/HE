#ifndef APP_H
#define APP_H
//
// C++ Interface: App
//
// Description:
//
//
// Author: Iztok Zupet <iz@elsis.si>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class App{
public:
    App();
    void run();        // main application loop
	static unsigned char val;

    static App app;
private:
    void dmxScreen();
    void localScreen();
};
#endif
