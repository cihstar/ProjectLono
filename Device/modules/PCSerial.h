#ifndef __PC_PRINT_H__
#define __PC_PRINT_H__

#include "mbed.h"
#include "rtos.h"
#include "util.h"
#include "PCCommand.h"

#include <string>
#include <vector>

class PCSerial{
    public:
        PCSerial(PinName tx, PinName rx, uint8_t size);
        ~PCSerial();        
        void setEcho(bool e);
        void setDebug(bool d);
        bool getDebug();
        string* getNextMessage();
        void print(string s);
        void startMatlab();
        void printMatlab(int s);
        void setEnableInput(bool b);
        void addCommand(PCCommand c);
        bool getGsmMode();
        void setGsmMode(bool b);
        void rxTask();
        Serial ser;
    private:
        void rxByte();
        void addToBuffer(char c);
        bool echo;
        char buffer[128];
        uint8_t count;                
        bool debug;  
        bool gsmMode;
        uint8_t bufferSize;             
        bool enableInput;
        bool messageStarted;
        
        Queue<string, 8> messageQueue;
        vector<PCCommand> commandList;
};

#endif