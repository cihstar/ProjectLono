#ifndef __BATTERY_LEVEL_H__
#define __BATTERY_LEVEL_H__

#include "mbed.h"
#include "rtos.h"

class BatteryLevel
{
    public:
        BatteryLevel(PinName input);
        ~BatteryLevel();
        void startTimer(uint16_t t);
        float read();
    private:
        AnalogIn battery;
        RtosTimer timer;
        void timerTask();    
        static void timerStarter(void const *p);
};

#endif