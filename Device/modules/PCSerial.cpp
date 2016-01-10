#include "PCSerial.h"
#include "util.h"

PCSerial::PCSerial(PinName tx, PinName rx, uint8_t size) : ser(tx,rx), echo(true){
    ser.attach(this,&PCSerial::rxByte);
    count = 0;
    typeDone = false;
    debug = true;
    bufferSize = size;
}

PCSerial::~PCSerial(){}

void PCSerial::send(PCMessage m)
{
    ser.printf("%s %s", m.getMessageType(), m.getInstruction());
}

void PCSerial::addToBuffer(char* buff, char c)
{
    if (count >= bufferSize)
        buff[15] = c;
    else
        buff[count] = c;
    count++;
}

void PCSerial::rxByte()
{
    char c = ser.getc();
    if (echo)
        ser.printf("%c",c); 
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
            addToBuffer(typeBuffer, '\0');
            count = 0;
            typeDone = true;
        }
        else if ( c == '\r' )
        {
            ser.printf("\n");
            addToBuffer(typeBuffer, '\0');
            count = 0;
            PCMessage newm(ToString(typeBuffer), "");
            messageQueue.put(&newm);  
        }
        else if ( c >= 33 && c <= 126)
        {
            addToBuffer(typeBuffer, c);
        }
    }
    else
    {
        if ( c == '\r')
        {
            ser.printf("\n");
            addToBuffer(instructionBuffer, '\0');   
            count = 0;
            typeDone = false;
            PCMessage newm(ToString(typeBuffer), ToString(instructionBuffer));
            messageQueue.put(&newm);
        }
        else if ( c >= 33 && c <= 126)
        {
            addToBuffer(instructionBuffer, c); 
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
    ser.printf("%s\r\n",s.c_str());
}

PCMessage::PCMessage(string t, string i)
{
    instruction = i;
    type = t;
}

PCMessage::PCMessage()
{
    type = "";
    instruction = "";
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

void PCMessage::setInstruction(string i)
{
    instruction = i;
}

string PCMessage::getInstruction()
{
    return instruction;
}

void printInfo(string s)
{   
    pc.print("Info: "+s);
}

void printDebug(string s)
{
    #ifdef DEBUG
        if (pc.getDebug())
            pc.print("Debug: "+s);
    #endif
}

void printError(string s)
{
    pc.print("Error: "+s);
}