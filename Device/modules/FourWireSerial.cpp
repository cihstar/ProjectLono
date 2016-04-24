/* Extension of the serial object to support an RTS and CTS line.
    However, never really used, as the GSM Module comms were reliable 
    with just tx and rx lines.
    
    Characters recieved are added to a circular buffer.
    When /r/n is recieved notifes the GSM recieve thread.
*/

#include "mbed.h"
#include "FourWireSerial.h"
#include "rtos.h"
#include "util.h"
#include <string>

FourWireSerial::FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts) :
RTS(rts), CTS(cts), serial(tx,rx), lastC(' '), setup(false),
readIndex(0), writeIndex(0)
{
    serial.attach(this, &FourWireSerial::recieveByte);
    setBaud(9600);        
}

void FourWireSerial::setRxThread(Thread *t, int* a)
{
    /* Save pointers to the GSM thread */
    messagesAvailable = a;
    rxThread = t;
    setup = true;
}

FourWireSerial::~FourWireSerial()
{
}

char FourWireSerial::getChar()
{
    /* Write to circular buffer */
    char c = charBuffer[readIndex];
    readIndex++;
    readIndex %= FWS_BUFFER_LENGTH;
    return c; 
}

void FourWireSerial::clearBuffer()
{
    /* Delete the buffer */
    for (int i = 0; i < FWS_BUFFER_LENGTH; i++)
    {
        charBuffer[i] = ' ';
    }    
    writeIndex = 0;
    readIndex = 0;
}

void FourWireSerial::recieveByte()
{    
    if(setup)
    {        
        char c = serial.getc();     
        
        /* Write to circular buffer */
        charBuffer[writeIndex] = c;
        writeIndex++;   
        writeIndex %= FWS_BUFFER_LENGTH;  
               
        if (lastC == '\r' && c == '\n')
        {
            /* End of message. Tell GSM thread */
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

/*********************************
GSMMessage Class
*********************************/

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