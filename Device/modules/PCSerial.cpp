#include "PCSerial.h"
#include "modules.h"

PCSerial::PCSerial(PinName tx, PinName rx, uint8_t size) : ser(tx,rx), echo(true), 
rxThread(&PCSerial::threadStarter, this, osPriorityNormal,3296), newm(), enableInput(false)
{
    ser.attach(this,&PCSerial::rxByte);
    count = 0;
    insCount = 0;
    typeDone = false;
    debug = true;
    bufferSize = size;
    gsmMode = false;
    rxThread.signal_set(1);  
    messageStarted = false;   
}

void PCSerial::threadStarter(void const *p) {
  PCSerial *instance = (PCSerial*)p;
  instance->rxTask();
}

bool PCSerial::getGsmMode()
{
    return gsmMode;
}

void PCSerial::setGsmMode(bool b)
{
    gsmMode = b;
}

void PCSerial::rxTask()
{
    rxThread.signal_wait(1);    
    PCMessage* m;
    string mType;
    std::vector<string> mIns;
    string full;
    bool success;
    while(true)
    {      
        /* Get Command and build info */              
        m = modules::pc->getNextMessage();
        mType = m->getMessageType();
        full = mType;
        mIns.clear();
        for (int i = 0; i < m->getLength(); i++)
        {
            mIns.push_back(m->getInstruction(i));   
            full+= " " + mIns[i];         
        }
        
        /* Log to SD card */
        modules::sdCard->writeToLog("PC: " + full);
        
        /* Special command handling */
        if (gsmMode)
        {
            /* Disabling GSM Direct Mode */
            if (mType == "directMode")
            {
                util::printBreak();
                util::print("Direct Mode OFF");
                modules::gsm->setPrint(false);
                gsmMode = false;
            }
            else
            {
                modules::gsm->sendCommandNoReply(mType);
            }
        }  
        else if (mType == "help")
        {
            /* Build help from list */
            util::printInfo("Project Lono - Smart Rain Gauge");
            util::printInfo("Chris Holbrow - University of Southampton - ECS Third Year Project 2016");
            util::printInfo("cih1g13@soton.ac.uk");
            util::printInfo("Samples data from pressure sensor to calculate the volume of water in the atached tube and then calculates the rainfall from this. Data is transmitted to server via 2G GSM mobile networks, with the data accessible at http://lono-rain.appspot.com");
            util::printInfo("Available Commands:");
            for (int i = 0; i < commandList.size(); i++)
            {
                 util::printInfo("Command '"+commandList[i].getName()+"' help: " + commandList[i].getDescription());
            }
            util::printBreak();    
        }
        else
        {    
            /* Handle all other commands added to list */   
            success = false;
            for (int j = 0; j < commandList.size(); j++)
            {
                if (mType.compare(commandList[j].getName()) == 0)                                
                {
                    success = true;
                    if (mIns[0] == "?")
                    {
                        /* Show help for this command */
                        util::printInfo("Command '"+mType+"' help: " + commandList[j].getDescription());
                        break;
                    }
                    if (commandList[j].getArgumentNumber() == mIns.size())
                    {
                        commandList[j].execute(mIns);                        
                        break;
                    }
                    else
                    {
                        /* Wrong nuber of commands */
                        util::printError("Wrong number of arguments for command: " + mType +". Accepts " + util::ToString(commandList[j].getArgumentNumber()) + ", " + util::ToString(mIns.size()) + " given.");
                        break;
                    }
                }
            }
            
            if (!success)
            {
                util::printError("Unknown Command '"+mType+"'");
            }
        }
    }
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

void PCSerial::addCommand(PCCommand c)
{
    commandList.push_back(c);
}

void PCSerial::rxByte()
{
    if (!enableInput) return;
    char c = ser.getc();
    if (!messageStarted)
    {
        messageStarted = true;
        ser.printf("\r\n");
    }
    if (echo) ser.printf("%c",c); 
    if(gsmMode)
    {
        if(c=='\r')
        {
            addToBuffer('\0');
            count = 0;            
            newm.clearInstruction();            
            count = 0;
            newm.setMessageType(util::ToString(buffer));
            messageQueue.put(&newm);  
            messageStarted = false;
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
            messageQueue.put(&newm);  
            messageStarted = false;
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
            count = 0;
            insCount = 0;
            typeDone = false;
            messageQueue.put(&newm);
            messageStarted = false;
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
    ser.printf("\r\n%s",s.c_str());    
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