#ifndef __FOUR_WIRE_SERIAL_H__
#define __FOUR_WIRE_SERIAL_H__

#include "mbed.h"
#include "rtos.h"
#include <string>

#define BUFFER_LENGTH 1023

class FourWireSerial {
   public:
    FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts);
    ~FourWireSerial();
    
    void sendData(string data);
    string getBuffer();
    uint16_t getBufferLength();
    void setBaud(int baud);
    
  private:
    DigitalIn CTS;
    DigitalOut RTS;   
    Serial serial;
    Semaphore sBuffer;
    char buffer[BUFFER_LENGTH];
    uint16_t bufferLen;
    uint16_t bufferFront;
    char getByteFromBuffer();
    void addByteToBuffer(char byte);
    void sendByte(char byte);
    void recieveByte();
    void *recieveBytePtr();
    void emptyBuffer();
};

#endif