#include "PCSerial.h"

PCSerial::PCSerial(PinName tx, PinName rx, uint8_t size) : ser(tx,rx), echo(true), 
rxThread(&PCSerial::threadStarter, this, osPriorityNormal,1024), newm(), enableInput(false)
{
    ser.attach(this,&PCSerial::rxByte);
    count = 0;
    insCount = 0;
    typeDone = false;
    debug = true;
    bufferSize = size;
    gsmMode = false;
    rxThread.signal_set(1);     
}

void PCSerial::threadStarter(void const *p) {
  PCSerial *instance = (PCSerial*)p;
  instance->rxTask();
}

PCSerial::~PCSerial(){}

void PCSerial::send(PCMessage m)
{
    ser.printf("%s %s", m.getMessageType(), m.getInstruction(0));
}

void PCSerial::addToBuffer(char c)
{
    if (count >= bufferSize)
        buffer[bufferSize-1] = c;
    else
        buffer[count] = c;
    count++;
}

void PCSerial::rxByte()
{
    if (!enableInput) return;
    char c = ser.getc();
    if (echo) ser.printf("%c",c); 
    if ( c == 8 || c == 127)
    {
        //backspace
        if (count > 0)
        {
            count--;
        }
    }
    if (!typeDone)
    {
        if ( c == ' ')
        {
            addToBuffer('\0');
            count = 0;
            typeDone = true;
            newm.setMessageType(util::ToString(buffer));
        }
        else if ( c == '\r' )
        {
            ser.printf("\n");
            addToBuffer('\0');
            count = 0;
            insCount = 0;
            newm.setMessageType(util::ToString(buffer));
            newm.setInstruction(0,"");
            newm.setInstruction(1,"");
            newm.setInstruction(2,"");
            messageQueue.put(&newm);  
        }
        else if ( c >= 33 && c <= 126)
        {
            addToBuffer(c);
        }
    }
    else
    {
        if ( c == '\r')
        {
            ser.printf("\n");
            addToBuffer('\0');
            newm.setInstruction(insCount, util::ToString(buffer));
            insCount++;
            for (int i = insCount; i < 3; i++)
            {
                newm.setInstruction(i,"");
            }
            count = 0;
            insCount = 0;
            typeDone = false;
            messageQueue.put(&newm);
        }
        else if ( c == ' ' )
        {
            addToBuffer('\0');
            count = 0;
            newm.setInstruction(insCount,util::ToString(buffer));
            insCount++;
        }
        else if ( c >= 33 && c <= 126)
        {
            addToBuffer(c); 
        }
    }
}

void PCSerial::setDebug(bool d)
{
    debug = d;
}

bool PCSerial::getDebug()
{
    return debug;
}

PCMessage* PCSerial::getNextMessage()
{    
    osEvent e = messageQueue.get();
    if (e.status == osEventMessage)
    {
        PCMessage *message = (PCMessage*)e.value.p;
        return message;
    }
}

void PCSerial::setEcho(bool e)
{
    echo = e;
}

void PCSerial::print(string s)
{
    //write to log.txt on sd card
    ser.printf("%s\r\n",s.c_str());    
}

void PCSerial::setEnableInput(bool b)
{
    enableInput = b;
}

PCMessage::PCMessage(string t, string i, string i1, string i2)
{
    instruction[0] = i;
    instruction[1] = i1;
    instruction[2] = i2;
    type = t;
}

PCMessage::PCMessage()
{
    type = "";
    instruction[0] = "";
    instruction[1] = "";
    instruction[2] = "";
}
PCMessage::~PCMessage(){}

void PCMessage::setMessageType(string t)
{
    type = t;
}

string PCMessage::getMessageType()
{
    return type;
}

void PCMessage::setInstruction(int x, string i)
{
    instruction[x] = i;
}

string PCMessage::getInstruction(int x)
{
    return instruction[x];
}