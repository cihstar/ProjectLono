#include "modules.h"
#include "util.h"
#include "PCSerial.h"

void PCSerial::rxTask()
{
    rxThread.signal_wait(1);    
    PCMessage* m;
    string mType;
    string mIns[3];
    while(true)
    {        
        m = modules::pc->getNextMessage();
        mType = m->getMessageType();
        mIns[0] = m->getInstruction(0);
        mIns[1] = m->getInstruction(1);
        mIns[2] = m->getInstruction(2);
        if (mType == "help")
        {
            util::printInfo("todo: Write Help.");
        }
        else if (mType == "setRainMode")
        {
            util::printInfo("Setting rain mode to " + mIns[0]);
        }
        else if (mType == "flash")
        {
            int i = util::ToInt(mIns[0]);
            if (i < 4)
            {
                util::printInfo("Toggling flasher "+mIns[0]);
                modules::flasher[i]->toggle();
            }
            else
            {
                util::printError("No flasher of that number");  
            }
        }
        else if (mType == "flashRate")
        {
            int i = util::ToInt(mIns[0]);
            int r = util::ToInt(mIns[1]);
            if (i < 4)
            {
                if (r >= 0)
                {
                    util::printInfo("Setting rate of flasher "+mIns[0]+" to "+mIns[1]+" ms");
                    modules::flasher[i]->setRate(r);
                }
                else
                {
                    util::printError("Specify value for flash rate");
                }
            }
            else
            {
                util::printError("No flasher of that number");
            }
        }
        else if (mType == "debug")
        {
            if (mIns[0] != "0")
            {
                util::printDebug("Enable Debug Mode");
                modules::pc->setDebug(true);
            }
            else
            {
                util::printInfo("Disable Debug Mode");
                modules::pc->setDebug(false);
            }
        }
        else if (mType == "getTime")
        {
            util::printInfo("System Time -> "+util::getTimeStamp());
        }
        else if (mType == "setTime")
        {
           bool r = util::setTime(mIns[0], mIns[1]);
           if (r)
           {               
               util::printInfo("Time Set Successfully. Current System Time: "+util::getTimeStamp());
           }
           else
           {
               util::printError("Incorrect Time/Date input. Use mm/dd/yyyy hh:mm:ss");
           }
        }
        else
        {
            util::printError("Unknown Command '"+mType+"'");
        }
    }
}

PCSerial::PCSerial(PinName tx, PinName rx, uint8_t size) : ser(tx,rx), echo(true), 
rxThread(&PCSerial::threadStarter, this, osPriorityNormal,1024), newm()
{
    ser.attach(this,&PCSerial::rxByte);
    count = 0;
    insCount = 0;
    typeDone = false;
    debug = true;
    bufferSize = size;
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