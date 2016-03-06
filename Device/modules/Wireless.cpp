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
         
         
        /* Device Info */
        string deviceId = "0";
        string serverUrl = "http://lono-rain.appspot.com";
        
        /* Configure connection to server */
        if (!(modules::gsm->configureServerConnection(serverUrl)))
        {
            /* Failed */
            util::printError("Unable to configure server connection for URL = "+serverUrl);
            util::printInfo("Done");
            return;
        }        
        util::printInfo("Server connection successfully configured to "+serverUrl);
        
        /* Register with the server */
        if (modules::gsm->httpPost("/reg","id:"+deviceId) == "Done")
        {
            util::printInfo("Successfully registered with server.");
            util::printInfo("Device ID = "+deviceId);
        }
        else
        {
            /* Setting mode to GSM failed */
            util::printError("Could not register with server.");
            util::printInfo("Done");
            return;
        }          
    }
    mode = t;
    util::printInfo("Done");
}

void Wireless::sendReadings()
{
    Wireless::Reading* r;
    while(1)
    {
        r = modules::pressureSensor->getNextReading();
        modules::sdCard->writeReading(*r);
        util::printInfo("Reading at: " + r->time + " for interval " + util::ToString((r->interval)/1000) + "s is " + util::ToString(r->value)+"mm");        
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

void Wireless::getTime()
{
    if (mode == GSM)
    {
        string r = modules::gsm->httpGet("/time");
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
