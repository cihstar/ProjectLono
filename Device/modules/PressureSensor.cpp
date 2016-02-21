#include "PressureSensor.h"
#include "modules.h"
#include "util.h"
#include <math.h>

PressureSensor::PressureSensor(PinName out, PinName sleep) :
sensor(out), sleepPin(sleep), timer(timerStarter, osTimerPeriodic, this), samples(0),
totalRain(0), emptying(false), lasth(0), sampsPerTx(0), readsPerSamp(0), tubeArea(0), 
outTubeArea(0), funnelRatio(0), startEmptyHeight(0), endEmptyHeight(0), calibration(0),
offset(0)
{
}

PressureSensor::~PressureSensor(){}

void PressureSensor::timerStarter(void const* p)
{
    PressureSensor *instance = (PressureSensor*)p;
    instance->timerTask();
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
    timer.start(sampInterval);
    util::printInfo("Pressure Sensor Readings Started");
    util::printInfo("Tube Area: " + util::ToString(tubeArea) + " m^2");
    util::printInfo("Out Tube Area: " + util::ToString(outTubeArea) + " m^2");
    util::printInfo("Funnel Ratio: " + util::ToString(funnelRatio));
    util::printInfo("Calibration Factor: " + util::ToString(calibration));
    util::printInfo("Calibration Offset: " + util::ToString(offset));
}

void PressureSensor::stopTimer()
{
    util::printInfo("Pressure Sensor Timer Stopped");
    timer.stop();
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
    calibration = c.fullHeight / (c.fullAdc - c.emptyAdc);
    offset = c.emptyAdc;
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
    wakeup();
    float h = toHeight(read());   
    
    //util::printDebug("Reading: " + util::ToString(h));
    
    if (emptying)
    {
        if (h <= endEmptyHeight)
        {
       //     util::printInfo("Tube Stopped Emptying");
            emptying = false;
        }
    }
    else
    {
        if (h >= startEmptyHeight)
        {
        //    util::printInfo("Tube Emptying");
            emptying = true;
        }
    }
    
    float waterOut = 0;
    if (emptying)
    {
        waterOut = calcTubeOut(h);
    }    
    
    totalRain += h + waterOut - lasth;
    
    lasth = h;
    samples++;
    
    if (samples == sampsPerTx)
    {
        float reading = totalRain * funnelRatio; 
        util::printInfo("Rainfall reading to send. " + util::ToString(reading) +" mm of rain since last send");
        modules::sdCard->writeReading(util::ToString(reading));   
        modules::gsm->httpGet("/time");     
        samples = 0;
        totalRain = 0;
    }
    sleep();
}

float PressureSensor::calcTubeOut(float h)
{
    return ((sqrt( 2 * pow(outTubeArea,2) * h * G) * (sampInterval / 1000)) / tubeArea);
}