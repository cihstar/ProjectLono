/* PCSerial Object allowing serial communications and control via a PC */

#include "PCSerial.h"
#include "modules.h"

PCSerial::PCSerial(PinName tx, PinName rx, uint8_t size) : ser(tx,rx), echo(true), enableInput(false),
count(0), debug(true), bufferSize(size), gsmMode(false), messageStarted(false)
{
    /* Serial rx interrupt */
    ser.attach(this,&PCSerial::rxByte);                          
}

/* GSM mode allows direct seial comms with the GSM module */
bool PCSerial::getGsmMode()
{
    return gsmMode;
}

void PCSerial::setGsmMode(bool b)
{
    gsmMode = b;
}

/* Services incoming messages that are in the thread */
void PCSerial::rxTask()
{          
    std::vector<string> mIns;
    string full;
    string mType;
    bool success;
    bool parsed;
    int space;
    
    /* Get Command and delete ptr */              
    string* msgptr = getNextMessage();
    if (msgptr == NULL)
    {
        /* No new message */
        delete msgptr;
        return;
    }
    
    full = *msgptr;
    delete msgptr;  
    
   /* Log to SD card */
    modules::sdCard->writeToLog("PC: " + full);
    
    /* Take args out of string - ' ' seperator */
    mIns.clear();
    parsed = false;
    while (!parsed)
    {            
        space = full.find(' ');            
        if (space == string::npos)
        {
            /* No space */
            mIns.push_back(full);
            parsed = true;
        }
        else
        {
            /* Space found */
            mIns.push_back(full.substr(0,space));
            full = full.substr(space+1);                
        }
    }                
    
    /* First element of vector is mType */
    mType = mIns[0];                
    mIns.erase(mIns.begin());                      
    
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
            if (mType == commandList[j].getName())                                
            {                    
                success = true;
                if (mIns.size() > 0)
                {
                    if (mIns[0] == "?")
                    {
                        /* Show help for this command */
                        util::printInfo("Command '"+mType+"' help: " + commandList[j].getDescription());
                        break;
                    }
                }
                if (commandList[j].getArgumentNumber() == mIns.size())
                {                        
                    commandList[j].execute(mIns);                        
                    break;
                }
                else
                {
                    /* Wrong nuber of arguments */
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

PCSerial::~PCSerial(){}

/* Save incoming char to buffer */
void PCSerial::addToBuffer(char c)
{
    if (count >= bufferSize)
        buffer[bufferSize-1] = c;
    else
        buffer[count] = c;
    count++;
}

/* Add command to command list */
void PCSerial::addCommand(PCCommand c)
{
    commandList.push_back(c);
}

/* Rx byte intterupt */
void PCSerial::rxByte()
{
    if (!enableInput) return;
    
    char c = ser.getc();
    
    if (!messageStarted)
    {
        messageStarted = true;
        count = 0;
        ser.printf("\r\n");
    }
    
    if (echo) ser.printf("%c",c); 
    
    if ( c == 8 || c == 127)
    {
        /* Backspace */
        if (count > 0)
        {
            count--;
        }
    }
    
    if ( c == '\r')
    {
        /* End of message */
        buffer[count] = '\0';
        string* message = new string(buffer);
        messageQueue.put(message);
        messageStarted = false;
    }
    else
    {
        /* Message contents */
        addToBuffer(c);
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

/* Get message from queue */
string* PCSerial::getNextMessage()
{    
    osEvent e = messageQueue.get(10);
    if (e.status == osEventMessage)
    {
        string *message = (string*)e.value.p;
        return message;
    }
    return NULL;
}

void PCSerial::setEcho(bool e)
{
    echo = e;
}

void PCSerial::print(string s)
{    
    ser.printf("\r\n%s",s.c_str());    
}

void PCSerial::startMatlab()
{
    ser.printf("\n");
}

void PCSerial::printMatlab(int s)
{
    ser.printf("%d\n",s);
}

void PCSerial::setEnableInput(bool b)
{
    enableInput = b;
}