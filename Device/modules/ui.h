#ifndef __UI_H__
#define __UI_H__

#include "mbed.h"
#include <string>

class UI
{
    public:
        UI(PinName sda, PinName scl, PinName rst, PinName b1, PinName b2, PinName b3);
        ~UI();    
        void writeText(string text);    
        void displayOn();
        void displayOff();
    private:  
         I2C lcd;
         InterruptIn buttons[3];   
         DigitalOut reset;  
         bool sendCommand(char byte, bool rs, bool rw);   
         void returnHome();
         void clearDisplay();
         void setEntryMode(bool p);
         void waitOnBusy();
};


#endif