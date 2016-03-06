#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include "mbed.h"
#include "rtos.h"

#include <string>

namespace Wireless
{
    enum ConnectionType
    {
        GSM,
        XBEE,
        NONE
    };
    
    struct Reading
    {
        float value;
        int interval; //seconds that reading is for
        string time;
    };
    
    void init();
    void setConnectionMode(Wireless::ConnectionType t);   
    void txReading(float reading);
    void sendThread(void const* p);
    Wireless::ConnectionType getConnectionMode();
    string getConnectionModeString();
    void getTime();
    
    static ConnectionType mode;
    static Queue<Reading, 12> wirelessTxQueue;
}

#endif