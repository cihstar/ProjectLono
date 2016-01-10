#ifndef __PC_PRINT_H__
#define __PC_PRINT_H__

#include "mbed.h"
#include "rtos.h"
#include <string>

class PCMessage{
  public:
    PCMessage(string t, string i);
    PCMessage();
    ~PCMessage();
   
    void setMessageType(string t);
    string getMessageType();
    
    void setInstruction(string t);
    string getInstruction();
    
    private:      
        string type;
        string instruction;
};

class PCSerial{
    public:
        PCSerial(PinName tx, PinName rx, uint8_t size);
        ~PCSerial();
        void send(PCMessage m);
        void setEcho(bool e);
        void setDebug(bool d);
        bool getDebug();
        PCMessage* getNextMessage();
        void print(string s);
        Queue<PCMessage, 8> messageQueue;
    private:
        Serial ser;
        void rxByte();
        void addToBuffer(char* buff, char c);
        bool echo;
        char typeBuffer[16];
        char instructionBuffer[16];
        uint8_t count;
        bool typeDone;
        bool debug;  
        uint8_t bufferSize;      
};

void printInfo(string s);
void printDebug(string s);
void printError(string s);

extern PCSerial pc;

#endif