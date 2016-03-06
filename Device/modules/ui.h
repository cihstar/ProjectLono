#ifndef __UI_H__
#define __UI_H__

#include "mbed.h"
#include "rtos.h"
#include "SB1602E.h"
#include "PinDetect.h"
#include <string>
#include <vector>

#define SCREEN_TIMEOUT 15000

class UI
{
    public:
        UI(PinName sda, PinName scl, PinName rst, PinName b1, PinName b2, PinName b3);
        ~UI();    
        void writeText(string line1, string line2 = "");
        void startedUp();
        void showMenu();
    private:            
         PinDetect dbuttons[3];  
         DigitalOut reset;      
         SB1602E* lcd;
         
         void screenOn();
         void screenOff();  
         
         void button1Push();
         void button2Push();
         void button3Push(); 
         
         bool menuActive;
         int activeMenuItem;
         vector<string> menuItems;
         void menuUp();
         void menuDown();
         
         void printStatus();
         
         RtosTimer timer;
         static void timerStarter(const void* p);
         void timerTask();
};


#endif