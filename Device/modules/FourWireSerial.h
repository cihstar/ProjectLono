#ifndef __FOUR_WIRE_SERIAL_H__
#define __FOUR_WIRE_SERIAL_H__

#include "mbed.h"
#include "rtos.h"
#include <string>

class FourWireSerial {
   public:
    FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts);
    ~FourWireSerial();
    
    void sendData(string data);
    void setBaud(int baud);
    char* getNextChar();
    
  private:
    DigitalIn CTS;
    DigitalOut RTS;   
    Serial serial;
    Queue<char, 512> charQueue;
    void recieveByte();
    void sendByte(char byte);
};

#endif