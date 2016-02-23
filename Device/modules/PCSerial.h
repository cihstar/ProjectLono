#ifndef __PC_PRINT_H__
#define __PC_PRINT_H__

#include "mbed.h"
#include "rtos.h"
#include "util.h"
#include <string>

#include <vector>

class PCMessage{
  public:
    PCMessage(string t);
    PCMessage();
    ~PCMessage();
   
    void setMessageType(string t);
    string getMessageType();
    
    void setInstruction(int x, string t);
    string getInstruction(int x);
    void addInstruction(string i);
    void clearInstruction();
    int getLength();
    
    private:      
        string type;
        std::vector<string> instruction;
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
        void setEnableInput(bool b);
    private:
        Serial ser;
        void rxByte();
        void addToBuffer(char c);
        bool echo;
        char buffer[128];
        uint8_t count;
        uint8_t insCount;
        bool typeDone;
        bool debug;  
        bool gsmMode;
        uint8_t bufferSize;  
        Thread rxThread;     
        static void threadStarter(void const *p);
        void rxTask();  
        PCMessage newm;    
        bool enableInput;
};

#endif