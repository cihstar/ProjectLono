#ifndef __XBEE_H__
#define __XBEE_H__

#include "mbed.h"
#include "rtos.h"
#include <string>

#define LENGTH 128

class XBEE
{
    public:
        XBEE(PinName tx, PinName rx);
        void send(string str);
        string getReply();
    
    private:
        Serial xbee;
        void rxByte();
        
        char buffer[LENGTH];
        int read;
        int write;
        int msgAvailable;
        char getChar();
        
        RtosTimer timeoutTimer;
        bool timeout;
        void timeoutTask();
        static void timerStarter(void const *p);
};

#endif