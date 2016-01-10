#include "mbed.h"
#include "PCSerial.h"
#include "FourWireSerial.h"
#include "rtos.h"
#include <string>

FourWireSerial::FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts) :
RTS(rts), CTS(cts), sBuffer(1), serial(tx,rx), bufferLen(0), bufferFront(0)
{
    serial.attach(this, &FourWireSerial::recieveByte);
}

FourWireSerial::~FourWireSerial()
{
}

void FourWireSerial::recieveByte()
{
    char c = serial.getc();
    addByteToBuffer(c);
}

void FourWireSerial::addByteToBuffer(char byte)
{
    sBuffer.wait();   
    if (bufferLen < (uint16_t) BUFFER_LENGTH)
    {
        buffer[(bufferFront + bufferLen) % BUFFER_LENGTH] = byte;
        bufferLen++;
    }
    else
    {
        RTS = 0; //tell device to stop sending as buffer full
    }
    sBuffer.release();   
}

void FourWireSerial::setBaud(int baud)
{
    serial.baud(baud);
}

char FourWireSerial::getByteFromBuffer()
{
    sBuffer.wait();
    if (bufferLen > 0)
    {
        char byte = buffer[bufferFront];
        bufferFront++;
        bufferLen--;
        bufferFront %= BUFFER_LENGTH;
        RTS = 1; //space now so make sure RTS is high to recieve more
        sBuffer.release();
        return byte;
    }
    else
    {
        sBuffer.release();
        return 0;
    }
}

void FourWireSerial::sendByte(char byte)
{
    //while (CTS == 0); //wait for CTS is high to send
    serial.putc(byte);
}

void FourWireSerial::emptyBuffer()
{
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        buffer[i] = '\0';
        bufferLen = 0;
        bufferFront = 0;
    }
}

string FourWireSerial::getBuffer()
{
    if (bufferLen > 0)
    {
        sBuffer.wait();
        buffer[(bufferFront + bufferLen) % BUFFER_LENGTH] = '\0';
        string str(buffer);
        emptyBuffer();
        sBuffer.release();
        RTS = 1;
        return str;
   }
   return "";
}

void FourWireSerial::sendData(string data)
{
    for (uint16_t i = 0; i < data.length(); i++)
    {
        sendByte(data[i]);
    }
}

uint16_t FourWireSerial::getBufferLength()
{
    return bufferLen;
}

