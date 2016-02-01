#include "mbed.h"
#include "FourWireSerial.h"
#include "rtos.h"
#include <string>

FourWireSerial::FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts) :
RTS(rts), CTS(cts), serial(tx,rx), newm()
{
    serial.attach(this, &FourWireSerial::recieveByte);
}

FourWireSerial::~FourWireSerial()
{
}

void FourWireSerial::recieveByte()
{
    char c = serial.getc();
    if (c == '\n' && lastC == '\r')
    {        
        if (!messageStarted)
        {
            //Start of message
            messageStarted = true;
            len = 0;
            newm.resetLength();
        }
        else
        {
            //end of message
            messageStarted = false;
            charBuffer[len] = '\0';
            newm.setMessage(charBuffer, newm.getLength());
            messageQueue.put(&newm);
        }
    }
    else
    {        
        if (messageStarted)
        {
            //middle of message, add to buffer
            if (len >= FWS_BUFFER_LENGTH-1)
            {
                RTS = 0;
            }
            else
            {
                if (c == ',')
                {
                    if (!bracketOpen)
                    {
                        //new agrument
                         charBuffer[len] = '\0';
                         newm.setMessage(charBuffer, newm.getLength()
                         newm.upLength();
                         len = 0;   
                    }
                }
                
                else if (c == '(' && !bracketOpen)
                {
                    bracketOpen = true;
                    charBuffer[len] = c;
                    len++;
                }
                
                else if (c == ')' && bracketOpen )
                {
                    bracketOpen = false;
                    charBuffer[len] = c;
                    len++;
                }
                
                else if ( c == ' ' && lastC == ':')
                {
                    charBuffer[len] = '\0';
                    newm.setMessage(charBuffer, newm.getLength()
                    newm.upLength();  
                }
                
                else 
                {
                    charBuffer[len] = c;
                    len++;
                }
                RTS = 1;
            }
        }
    }
    
    // save this char
    lastC = c;
}

void FourWireSerial::setBaud(int baud)
{
    serial.baud(baud);
}

void FourWireSerial::sendByte(char byte)
{
    while (CTS == 0); //wait for CTS is high to send
    serial.putc((char) byte);
}

void FourWireSerial::sendData(string data)
{
    for (uint16_t i = 0; i < data.length(); i++)
    {
        sendByte(data[i]);
    }
}

GSMMessage* FourWireSerial::getNextMessage()
{
    osEvent e = messageQueue.get();
    if (e.status == osEventMessage)
    {
        GSMMessage* m = (GSMMessage*) e.value.p;
        return m;
    }
}

GSMMessage::GSMMessage(string m, int l) : message(m), length(l)
{
}
GSMMessage::GSMMessage():message(""), length(0)
{}

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

string GSMMessage::getMessage()
{
    return message;
}

void GSMMessage::setMessage(string m)
{
    message = m;
}
