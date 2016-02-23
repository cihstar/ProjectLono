#ifndef __UI_H__
#define __UI_H__

#include "mbed.h"
<<<<<<< HEAD
#include "rtos.h"
=======
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
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
<<<<<<< HEAD
         bool waitOnBusy();
         
         RtosTimer timeout;
         void timerTask();    
         static void timerStarter(void const *p);
         bool Timeout;
=======
         void waitOnBusy();
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
};


#endif