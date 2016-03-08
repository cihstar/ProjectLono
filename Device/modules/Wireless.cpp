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
        }
         
        /* Device Info */
        string deviceId = "0";
        string serverUrl = "http://lono-rain.appspot.com:80";
                
        /* Configure connection to server */
        if (!(modules::gsm->configureServerConnection(serverUrl)))
        {
            /* Failed */
            util::printError("Unable to configure server connection for URL = "+serverUrl);
            util::printInfo("Done");
            return;
        }        
        util::printInfo("Server connection successfully configured to "+serverUrl);
        
        wait(5);
        
        /* Register with the server */
        string datetime = modules::gsm->httpPost("/reg","id="+util::ToString(deviceId));
        //util::printDebug("returned date " + datetime);
        util::setTime(datetime.substr(0,10), datetime.substr(11,8));
        util::printInfo("Successfully registered with server.");
        util::printInfo("Device ID = "+deviceId);       
    }
    mode = t;
    util::printInfo("Done");
}

void Wireless::sendReadings()
{
    Wireless::Reading* r;
    string data;
    string time;
    int interval;
    float value;
    while(1)
    {
         /* Wait for new reading from queue */
         r = modules::pressureSensor->getNextReading();            
            
        /* Save to .csv file on SD Card */
        modules::sdCard->writeReading(*r);
        
        /* Get reading info */
        time = r->time;
        interval = (r->interval)/1000;
        value = r->value;
        
        /* Print reading to PC/Log */
        util::printInfo("Reading at: " + time + " for interval " + util::ToString(interval) + "s is " + util::ToString(value)+"mm");
        
        /* TX reading */
        if (mode == GSM)
        {                  
            data = "id=0&reading="+util::ToString(value)+"&interval="+util::ToString(interval)+"&time="+time;            
            modules::gsm->httpPost("/send", data);
            util::printInfo("TX to server was successful");
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