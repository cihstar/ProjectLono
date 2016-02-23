#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include "mbed.h"
#include "rtos.h"

#include <string>

namespace Wireless
{
    enum ConnectionType{NONE, GSM, XBEE};
        
    struct Reading
    {
        float value;
        string time;
    };

    void init();
    void setConnectionMode(ConnectionType t);
    ConnectionType getConnectionMode();
    void txReading(float reading);
    void getTime();     
    void sendThread(void const *p);

    static ConnectionType mode;
    static Queue<Reading, 12> wirelessTxQueue;
}

#endif