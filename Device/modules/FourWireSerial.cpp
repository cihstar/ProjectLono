#include "mbed.h"
#include "FourWireSerial.h"
#include "rtos.h"
#include "util.h"
#include <string>

FourWireSerial::FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts) :
RTS(rts), CTS(cts), serial(tx,rx), newm(), bracketOpen(false), len(0), lastC(' '), setup(false),
readIndex(0), writeIndex(0), LFCR(false)
{
    serial.attach(this, &FourWireSerial::recieveByte);
    setBaud(9600);
    DigitalOut m(p21); 
    m = 1;
}

void FourWireSerial::setRxThread(Thread *t, int* a)
{
    messagesAvailable = a;
    rxThread = t;
    setup = true;
}

FourWireSerial::~FourWireSerial()
{
}

char FourWireSerial::getChar()
{
    if (readIndex == writeIndex)
    {
        util::printDebug("Read > Write, R: "+util::ToString(readIndex)+" W: "+util::ToString(writeIndex));
        return 'E';
    }
    char c = charBuffer[readIndex];
    readIndex++;
    readIndex %= FWS_BUFFER_LENGTH;
    return c; 
}

void FourWireSerial::clearBuffer()
{
    for (int i = 0; i < FWS_BUFFER_LENGTH; i++)
    {
        charBuffer[i] = ' ';
    }
    len=0;
    writeIndex = 0;
    readIndex = 0;
}

void FourWireSerial::recieveByte()
{    
    if(setup)
    {        
        char c = serial.getc();     
        
        charBuffer[writeIndex] = c;
        writeIndex++;   
        writeIndex %= FWS_BUFFER_LENGTH;  
               
        if (lastC == '\r' && c == '\n')
        {
            rxThread->signal_set(FWS_MESSAGE_READY);     
            (*messagesAvailable)++;                         
        }        
        lastC = c;        
    }
}

void FourWireSerial::setBaud(int baud)
{
    serial.baud(baud);
}

void FourWireSerial::sendByte(char byte)
{        
    serial.putc((char) byte);
}

void FourWireSerial::sendData(string data)
{
    for (uint16_t i = 0; i < data.length(); i++)
    {
        sendByte(data[i]);
    }
}

GSMMessage::GSMMessage(string m, int l) : length(l)
{
    message[0] = m;
}
GSMMessage::GSMMessage():length(0)
{
    message[0] = "";
}

int GSMMessage::getLength()
{
    return length;
}

void GSMMessage::upLength()
{
    length++;
}

void GSMMessage::resetLength()
{
    length = 0;
}

string GSMMessage::getMessage(int i)
{
    return message[i];
}

void GSMMessage::addMessage(string m)
{
    message[length] = m;
    length++;
}

void GSMMessage::setMessage(string m, int i)
{
    if (i > length)
    {
        length = i;
    }
    message[i] = m;
}
