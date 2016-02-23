#include "PCSerial.h"

PCSerial::PCSerial(PinName tx, PinName rx, uint8_t size) : ser(tx,rx), echo(true), 
rxThread(&PCSerial::threadStarter, this, osPriorityNormal,2048), newm(), enableInput(false)
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
    if(gsmMode)
    {
        if(c=='\r')
        {
            addToBuffer('\0');
<<<<<<< HEAD
            count = 0;            
            newm.clearInstruction();            
=======
            count = 0;
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
            newm.setMessageType(util::ToString(buffer));
            messageQueue.put(&newm);  
        }
        else
        {
            addToBuffer(c);
        }
        return;
    }
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
            newm.clearInstruction();            
        }
        else if ( c == '\r' )
        {
            ser.printf("\n");
            addToBuffer('\0');
            count = 0;
            insCount = 0;
            newm.clearInstruction();
            newm.setMessageType(util::ToString(buffer));
<<<<<<< HEAD
=======
            for (int i = 0; i < M; i++)
            {
                newm.setInstruction(i,"");
            }
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
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
            newm.addInstruction(util::ToString(buffer));
            insCount++;
<<<<<<< HEAD
=======
            for (int i = insCount; i < M; i++)
            {
                newm.setInstruction(i,"");
            }
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
            count = 0;
            insCount = 0;
            typeDone = false;
            messageQueue.put(&newm);
        }
        else if ( c == ' ' )
        {
            addToBuffer('\0');
            count = 0;
            newm.addInstruction(util::ToString(buffer));
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

PCMessage::PCMessage(string t)
{    
    type = t;
}

PCMessage::PCMessage()
{    
    type = "";
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

void PCMessage::addInstruction(string i)
{
    instruction.push_back(i);
}

string PCMessage::getInstruction(int x)
{
    return instruction[x];
}

void PCMessage::clearInstruction()
{
    instruction.clear();
}

int PCMessage::getLength()
{
    return instruction.size();
}