#include "Wireless.h"
#include "util.h"
#include "pindef.h"
#include "modules.h"
#include "XBEE.h"
#include "GSM.h"

void Wireless::init()
{
    mode = Wireless::NONE;
    Thread txThread(Wireless::sendThread);
}

void Wireless::setConnectionMode(Wireless::ConnectionType t)
{
    util::printDebug("input t: " + util::ToString(t));
    util::printDebug("current mode: " + util::ToString(mode));
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
        //Init GSM Module.
         util::printInfo("Configuring GSM module...");
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

void Wireless::txReading(float reading)
{
    Reading r = {reading, util::getTimeStamp()};
    util::printInfo("Rainfall Reading at " + r.time + ": " + util::ToString(r.value) + "mm");
    wirelessTxQueue.put(&r);        
}

void Wireless::sendThread(void const *p)
{
    while(1)
    {
        if (mode == GSM)
        {
            Reading*  message;
            osEvent e = wirelessTxQueue.get();
            if (e.status == osEventMessage)
            {
               message = (Reading*)e.value.p;                                       
            
                string data = "r="+util::ToString(message->value)+"&time="+message->time;            
            
                if (!(modules::gsm->httpPost("/reading",data) == "Done"))
                {
                    //Failed so put back and try again.
                    wirelessTxQueue.put(message);
                }
            }
        }
    }       
}



Wireless::ConnectionType Wireless::getConnectionMode()
{
    return mode;
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
