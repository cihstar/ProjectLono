#ifndef __FOUR_WIRE_SERIAL_H__
#define __FOUR_WIRE_SERIAL_H__

#include "mbed.h"
#include "rtos.h"
#include <string>

#define FWS_BUFFER_LENGTH 512

class GSMMessage {
    private:
        string message;
        int length;
    public:
        GSMMessage(string m, int l);
        GSMMessage();
        int getLength();
        void setLength(int len);
        string getMessage();
        void setMessage(string m);
};

class FourWireSerial {
   public:
    FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts);
    ~FourWireSerial();
    
    void sendData(string data);
    void sendByte(char byte);       
    void setBaud(int baud);
    GSMMessage* getNextMessage();
    
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
};

#endif