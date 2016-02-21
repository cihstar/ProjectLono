#include "BatteryLevel.h"
#include "util.h"

BatteryLevel::BatteryLevel(PinName input) : 
timer(timerStarter, osTimerPeriodic, this),
battery(input)
{
    
};

BatteryLevel::~BatteryLevel(){}

void BatteryLevel::timerStarter(void const* p)
{
    BatteryLevel *instance = (BatteryLevel*)p;
    instance->timerTask();
}

void BatteryLevel::timerTask()
{
    util::printInfo("Battery Level: " + util::ToString(read()) + "V");
}

float BatteryLevel::read()
{
    return battery.read() * 3;
}

void BatteryLevel::startTimer(uint16_t t)
{
    timer.start(t);
}