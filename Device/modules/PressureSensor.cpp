#include "PressureSensor.h"
#include "modules.h"
#include "util.h"
#include "Wireless.h"
#include <math.h>

PressureSensor::PressureSensor(PinName out, PinName sleep) :
sensor(out), sleepPin(sleep), timer(&PressureSensor::timerStarter, this, osPriorityNormal,2048), samples(0),
totalRain(0), emptying(false), lasth(0), sampsPerTx(0), readsPerSamp(0), tubeArea(0), 
outTubeArea(0), funnelRatio(0), startEmptyHeight(0), endEmptyHeight(0), calibration(0.0f),
offset(0), active(false), lastReading("None"), reading(0)
{
}

PressureSensor::~PressureSensor(){}

void PressureSensor::timerStarter(void const* p)
{
    PressureSensor *instance = (PressureSensor*)p;
    instance->timerTask();
}

int PressureSensor::getTxInterval()
{
    return sampsPerTx * sampInterval;
}

void PressureSensor::setTiming(int tTx, int tSamp, int reads)
{
    sampsPerTx = tTx / tSamp;
    sampInterval = tSamp;
    readsPerSamp = reads;
}

void PressureSensor::start()
{
    if (tubeArea == 0 || funnelRatio == 0)
    {
        util::printError("Cannot start pressure readings. Tube physical dimensions not set");
        return;
    }
    if (calibration == 0)
    {
        util::printError("Cannot start pressure readings. Sensor not calibrated");
        return;
    }
    if (sampsPerTx == 0 || sampInterval == 0 || readsPerSamp == 0)
    {
        util::printError("Cannot start pressure readings. Timing not valid");
        return;
    }
    lasth = toHeight(read());
    samples = 0;        
    active = true;
    util::printInfo("Pressure Sensor Readings Started");
    util::printInfo("Tube Area: " + util::ToString(tubeArea) + " m^2");
    util::printInfo("Out Tube Area: " + util::ToString(outTubeArea) + " m^2");
    util::printInfo("Funnel Ratio: " + util::ToString(funnelRatio));
    util::printInfo("Calibration Factor: " + util::ToString(calibration));
    util::printInfo("Calibration Offset: " + util::ToString(offset));
}

bool PressureSensor::getActive()
{
    return active;
}

void PressureSensor::stopTimer()
{
    util::printInfo("Pressure Sensor Timer Stopped");    
    active = false;
}

float PressureSensor::area(float r)
{
    return M_PI * pow(r,2);
}

void PressureSensor::setDimensions(Dimensions d)
{
    outTubeArea = area(d.outTubeRadius);
    float outTubeWallArea = area(d.outTubeWall) - outTubeArea;
    tubeArea = area(d.tubeRadius) - outTubeWallArea - area(d.pressureSensorTubeRadius);
    funnelRatio = 1000 / (pow(d.funnelRadius,2) / pow(d.tubeRadius,2));
}

void PressureSensor::calibrate(Calibrate c)
{
    if (c.fullAdc != 0)
    {
        calibration = c.fullHeight / (c.fullAdc - c.emptyAdc);
        offset = c.emptyAdc;   
    }
}

uint16_t PressureSensor::read()
{
    uint32_t adcRead = 0;
    for (int i = 0; i < readsPerSamp; i++)
    {
        adcRead += (uint32_t) sensor.read_u16(); 
    }
    adcRead /= readsPerSamp;
    return (uint16_t) adcRead;
}

float PressureSensor::toHeight(uint16_t adcVal)
{
    return (((float) adcVal) - offset) * calibration;
}

void PressureSensor::sleep()
{
    sleepPin = 0;
}

void PressureSensor::wakeup()
{
    sleepPin = 1;
}


void PressureSensor::timerTask()
{   
   float h;
   float waterOut;
   string str;
   string time;
    
    while(1)
    {        
        //printf("\r\nPressure Sensor Thread Used stack: %d\r\n", timer.used_stack());
        while(!active)
        {            
            Thread::wait(100);
        }        
            
        wakeup();
        h = toHeight(read());   
        
        if (emptying)
        {
            if (h <= endEmptyHeight)
            {        
                emptying = false;
            }
        }
        else
        {
            if (h >= startEmptyHeight)
            {            
                emptying = true;
            }
        }
        
        waterOut = 0;
        if (emptying)
        {
            waterOut = calcTubeOut(h);
        }    
        
        totalRain += h + waterOut - lasth;
        
        lasth = h;
        samples++;
        
        if (samples == sampsPerTx)
        {                        
            reading = totalRain * funnelRatio;                          
            str = util::ToString(reading);                          
            lastReading = str;
            
            Wireless::Reading r = {reading, getTxInterval(), util::getTimeStamp()};
            readingQueue.put(&r);
            
            samples = 0;              
            totalRain = 0;                                    
        }
        sleep();         
        Thread::wait(sampInterval);
        
        /***********************************************
        ADD TIMER TOO, to set signal for thread to go
        ************************************************/
    }
}

Wireless::Reading* PressureSensor::getNextReading()
{
    osEvent e = readingQueue.get();
    if (e.status == osEventMessage)
    {
        return (Wireless::Reading*)e.value.p;        
    }   
}


string PressureSensor::getLastReading()
{
    return lastReading;
}

float PressureSensor::calcTubeOut(float h)
{
    return ((sqrt( 2 * pow(outTubeArea,2) * h * G) * (sampInterval / 1000)) / tubeArea);
}