#ifndef __FOUR_WIRE_SERIAL_H__
#define __FOUR_WIRE_SERIAL_H__

#include "mbed.h"
#include "rtos.h"
#include <string>
#include <memory>

#define FWS_MESSAGE_READY 0x02
#define FWS_BUFFER_LENGTH 1024

class GSMMessage {
    private:
        string message[8];
        int length;
    public:
        GSMMessage(string m, int l);
        GSMMessage();
        int getLength();
        void upLength();
        void resetLength();
        string getMessage(int i=0);
        void setMessage(string m, int i);
        void addMessage(string m);
};

typedef std::unique_ptr<GSMMessage> ptr_GSM_msg;

class FourWireSerial {
   public:
    FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts);
    ~FourWireSerial();
    
    void sendData(string data);
    void sendByte(char byte);       
    void setBaud(int baud);
    GSMMessage* getNextMessage();
    string printBuffer();
    void clearBuffer();
    void setRxThread(Thread* t, int* a);
    char getChar();
    
  private:
    DigitalIn CTS;
    DigitalOut RTS;   
    Serial serial;
    char charBuffer[FWS_BUFFER_LENGTH];
    Queue<GSMMessage, 16> messageQueue;
    void recieveByte();
    uint16_t len;
    char lastC;
    bool messageStarted;
    GSMMessage newm;
    bool bracketOpen;
    Thread* rxThread;
    bool setup;
    int* messagesAvailable;
    
    int readIndex;
    int writeIndex;
    
    bool LFCR;
};

#endif