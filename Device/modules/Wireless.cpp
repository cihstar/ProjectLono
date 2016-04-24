/********************************************************************************

   Rain Gauge | Wireless.cpp | Chris Holbrow
   
   ----------------------------------------------------------------------------

   Contains functions dealing with the transmissions of data from the device
   to remote server, acting as an abstraction layer for other modules to access
   wireless transmission, independent of mode selected.
   Current modes:
        = GSM  - Using GSM mobile network to communicate to server via HTTP
        = XBEE - Uses Zigbee RF protocol to transmit to a remote basestation
        = NONE - None selected, just logs to SD Card and nothing else        
        
**********************************************************************************/

#include "Wireless.h"
#include "util.h"
#include "pindef.h"
#include "modules.h"
#include "XBEE.h"
#include "GSM.h"

void Wireless::init()
{
    mode = Wireless::NONE;
}

void Wireless::setConnectionMode(Wireless::ConnectionType t)
{
    string deviceId = "0";
    if (t == mode)
    {
        //No change
        util::printInfo("Connection Mode already set to " + util::ToString(t));
        return;
    }
    
    if (t == Wireless::XBEE)
    {
        //Init XBEE module
        util::printInfo("Configuring XBEE Module...");
        util::printInfo("Done");
        mode = t;
    }
    
    if (t == Wireless::GSM)
    {        
        util::printInfo("Configuring GSM module...");
        int count = 0;
        while (!modules::gsm->isOn())
        {
            count++;
            if (count == 20)
            {
                util::printError("GSM modules appears to not be powered on");
                return;
            }         
            Thread::wait(250);  
        }
        //Init GSM Module.
         
        //Ensure Baud Rate is set to 9600
        ptr_GSM_msg m;
        m = modules::gsm->sendCommand("AT+IPR=9600",2);
       // util::printDebug("reply here: "  + m->getMessage(0));
        if (m->getMessage(1).find("OK") != string::npos)
        {
            util::printInfo("Set GSM Baud rate to 9600");
            // serial.setBaud(9600);
        }
        else
        {
            util::printError("Unable to set GSM Baud rate to 9600");
        }
        
        //Disable echo on GSM module
        m = modules::gsm->sendCommand("ATE",2);                    
        if (m->getMessage(1).find("OK") != string::npos)
        {
            util::printInfo("Disabled UART GSM echo");                        
        }
        else
        {
            util::printError("Unable to disable GSM UART echo");
        }
        
        //Enable error reporting
        m = modules::gsm->sendCommand("AT+CMEE=2");
        if (m->getMessage(0).find("OK") != string::npos)
        {
            util::printInfo("Enabled error reporting from GSM");                        
        }
        else
        {
            util::printError("Unable to enable error reporting from GSM");
        }
        
        /* Read SMS in text mode */
        m = modules::gsm->sendCommand("AT+CMGF=1");
        if (m->getMessage(0).find("OK") != string::npos)
        {
            util::printInfo("SMS Text Mode enabled");                        
        }
        else
        {
            util::printError("Unable to set SMS mode to text");
        }
        
        /* Wait for device to register with network */
        bool success = false;
        util::printInfo("Waiting for device to register with mobile network...");
        for (int i = 0; i < 10; i++)
        {
            m = modules::gsm->sendCommand("AT+CREG?",2);            
            if (m->getMessage(1).find("OK") != string::npos)
            {                
                if (m->getMessage(0).find("0,1") != string::npos)
                {
                    success = true;
                    break;
                }
            }            
            Thread::wait(1000);
        } 
        
        if (success)
        {
            util::printInfo("Device registered with network");
        }
        else
        {
            util::printError("Device not registered with network");
            return;
        }
        
        /* Register with the server */
        /* Send device id to server, get the current time as a reply */
        util::printInfo("Connecting to server...");
        string datetime = modules::gsm->httpPost("http://lono-rain.appspot.com:80","/reg","id="+util::ToString(deviceId));
        if (datetime.length() == 20)
        {
            util::setTime(datetime.substr(0,10), datetime.substr(11,8));
            util::printInfo("Successfully registered with server.");
            util::printInfo("Device ID = "+deviceId);
        }
        else
        {
            util::printError("Unable to register with server.");
            return;
        }       
    }
    else if (t == Wireless::XBEE)
    {
        /* Set up Xbee */
        /* Note: not fully tested */
        string reply;
        bool success = false;
        util::printInfo("Attempting to test link to XBEE host...");
        for (int i = 0; i < 5; i++)
        {
            /* Send and wait for reply from host to see if it is there */
            modules::xbee->send("REGISTER,id="+deviceId);            
            
            reply = modules::xbee->getReply();
            
            if (reply.find("Done") != string::npos)
            {
                util::printInfo("Registered with XBEE host");
                success = true;
                break;
            }
            else
            {
                util::printInfo("Failed. Reply was: " + reply + ". Attempting again...");
            }
        }
        
        if (!success)
        {
            util::printError("No Reply from XBEE host");
            return;            
        }
            
        /* Get time */
        modules::xbee->send("TIME");
        reply = modules::xbee->getReply();
        if (reply.length() == 20)
        {
            util::setTime(reply.substr(0,10), reply.substr(11,8));            
            util::printInfo("Retrieved time from Xbee host");
        }
        else
        {
            util::printError("Unable to get time from xbee host");
            return;
        }          
    }
    mode = t;
    util::printInfo("Done");
}

void Wireless::sendReadings()
{
    Wireless::Reading* r;
    string data;
    string time;
    string str;
    int interval;
    float value;
    
    /* Wait for new reading from queue */
    r = modules::pressureSensor->getNextReading();
    if (r == NULL)
    {
        /* No new message */
        return;
    }            
        
    /* Save to .csv file on SD Card */
    modules::sdCard->writeReading(*r);
    
    /* Get reading info */
    time = r->time;
    interval = (r->interval)/1000;
    value = r->value;
    
    /* Print reading to PC/Log */
    util::printInfo("Reading at: " + time + " for interval " + util::ToString(interval) + "s is " + util::ToString(value)+"mm" + " Rate = " + util::ToString(value * (3600/interval)) + " mm/h");
    
    /* TX reading */
    data = "id=0&reading="+util::ToString(value)+"&interval="+util::ToString(interval)+"&time="+time;
    
    /* Switch operation based upon mode selected */
    if (mode == GSM)
    {                  
        str = modules::gsm->httpPost("http://lono-rain.appspot.com:80","/send", data);            
        if (str.find("Done") != string::npos)
        {
            util::printInfo("TX to server was successful");
        }
        else
        {
            util::printError("Could not TX to server. The server returned: " + str);
        }
    }
    else if (mode == XBEE)
    {
        modules::xbee->send(data);
        str = modules::xbee->getReply();
        if (str.find("Done") != string::npos)
        {
            util::printInfo("XBEE transfer was successful");
        }   
        else
        {
            util::printError("No reply from Xbee host");
        }
    }
}

Wireless::ConnectionType Wireless::getConnectionMode()
{
    return mode;
}

string Wireless::getConnectionModeString()
{
    if (mode == Wireless::GSM)
    {
        return "GSM";
    }
    else if (mode == Wireless::XBEE)
    {
        return "XBEE";    
    }
    else
    {
        return "None";
    }
}


/* Currently returned by server after registering, idea to return from XBEE too? */
void Wireless::getTime()
{
    if (mode == GSM)
    {
        string r = modules::gsm->httpGet("http://lono-rain.appspot.com:80","/time");
        if (r.length() == 19)
        {
            util::setTime(r.substr(0,10),r.substr(11,8));
            util::printInfo("Successfully retrieved time from server");
        }
        else
        {
            util::printError("Could not retrieve time from server");
        }   
    }
}