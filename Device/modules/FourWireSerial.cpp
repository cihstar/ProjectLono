#include "mbed.h"
#include "FourWireSerial.h"
#include "rtos.h"
#include <string>

FourWireSerial::FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts) :
RTS(rts), CTS(cts), serial(tx,rx)
{
    serial.attach(this, &FourWireSerial::recieveByte);
}

FourWireSerial::~FourWireSerial()
{
}

void FourWireSerial::recieveByte()
{
    char c = serial.getc();
    charQueue.put(&c);
}

void FourWireSerial::setBaud(int baud)
{
    serial.baud(baud);
}

void FourWireSerial::sendByte(char byte)
{
    //while (CTS == 0); //wait for CTS is high to send
    serial.putc(byte);
}

void FourWireSerial::sendData(string data)
{
    for (uint16_t i = 0; i < data.length(); i++)
    {
        sendByte(data[i]);
    }
}

char* FourWireSerial::getNextChar()
{
    osEvent e = charQueue.get();
    if (e.status == osEventMessage)
    {
        char *message = (char*)e.value.p;
        return message;
    }
}
