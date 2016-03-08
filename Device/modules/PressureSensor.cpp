#include "PressureSensor.h"
#include "modules.h"
#include "util.h"
#include "Wireless.h"
#include <math.h>

PressureSensor::PressureSensor(PinName out, PinName sleep) :
sensor(out), sleepPin(sleep), senseThread(&PressureSensor::threadStarter, this, osPriorityNormal,1948), samples(0),
totalRain(0), emptying(false), lasth(0), sampsPerTx(0), readsPerSamp(0), tubeArea(0), 
outTubeArea(0), funnelRatio(0), startEmptyHeight(0), endEmptyHeight(0), calibration(0.0f),
offset(0), active(false), lastReading("None"), reading(0), timer(timerStarter, osTimerPeriodic, this),
equationConstant(0)
{
}

PressureSensor::~PressureSensor(){}

void PressureSensor::timerStarter(void const *p)
{
    PressureSensor *instance = (PressureSensor*)p;
    instance->timerTask();
}

void PressureSensor::timerTask()
{    
    senseThread.signal_set(SAMPLE);
}

void PressureSensor::threadStarter(void const* p)
{
    PressureSensor *instance = (PressureSensor*)p;
    instance->sensorTask();
}

uint32_t PressureSensor::getTxInterval()
{
    return sampsPerTx * sampInterval;
}

void PressureSensor::setTiming(Timing t)
{
    sampsPerTx = t.tx / t.samp;
    sampInterval = t.samp;
    readsPerSamp = t.reads;
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
    
    /* Calculate equation constant */
    equationConstant = sqrt(2 * pow(outTubeArea,2) * G) * (sampInterval / tubeArea);
    
    
    lasth = toHeight(read());
    samples = 0;        
    active = true;
    timer.start(sampInterval);
    util::printInfo("Pressure Sensor Readings Started");
    util::printInfo("Tube Area: " + util::ToString(tubeArea) + " m^2");
    util::printInfo("Out Tube Area: " + util::ToString(outTubeArea) + " m^2");
    util::printInfo("Funnel Ratio: " + util::ToString(funnelRatio));
    util::printInfo("Calibration Factor: " + util::ToString(calibration));
    util::printInfo("Calibration Offset: " + util::ToString(offset));
    
    util::printInfo("Transmission of Data every " + util::ToString(getTxInterval()/1000) + "s");
    util::printInfo("Sampling of Sensor every " + util::ToString(sampInterval) + "ms");
    util::printInfo("ADC reads to average per sample: " + util::ToString(readsPerSamp));
    
    util::printInfo("Now sampling....");    
}

bool PressureSensor::getActive()
{
    return active;
}

void PressureSensor::stopTimer()
{
    util::printInfo("Pressure Sensor Timer Stopped");    
    timer.stop();
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
    startEmptyHeight = d.startEmptyHeight;
    endEmptyHeight = d.endEmptyHeight;        
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


void PressureSensor::sensorTask()
{   
   float h;
   float waterOut;
   string str;
   string time;
    
    while(1)
    {         
        /* Wait until thread started */
        while(!active)
        {                        
            Thread::wait(1);
        }                     
        
        /* Turn on 5V regulator */    
        wakeup();
        
        /* Read ADC and convert to height */
        h = toHeight(read());   
        
        /* Constrain height to be > 0 */
        if (h < 0)
        {
            h = 0;
        }
        
        /* Check if tube will be emptying or not */
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
        //printf("Y");
        
        /* Calculate ammount of water that has left the tube */        
        if (emptying)
        {            
            waterOut = sqrt(h) * equationConstant;
        }    
        else
        {
            waterOut = 0;
        }
        
        /* Caclulate total rain in this sample and accumulate */
        totalRain += h + waterOut - lasth;
        
        /* Save */
        lasth = h;
        samples++;
        
        /* Time to tx sample */
        if (samples == sampsPerTx)
        {       
            /* Stop timer while processing */             
            timer.stop();    
            
            /* Convert height in tube to rainfall in mm */
            reading = totalRain * funnelRatio;                          
            str = util::ToString(reading);                          
            lastReading = str;
            
            /* Send reading */
            Wireless::Reading r = {reading, getTxInterval(), util::getTimeStamp()};
            readingQueue.put(&r);
            wait(0.1);
            
            /* Reset for next reading */
            samples = 0;              
            totalRain = 0;
            timer.start(sampInterval);                                               
        }
        
        /* Turn off 5V regulator */
        sleep();
        
        /* Wait for timer signal to smaple again */
        Thread::signal_wait(SAMPLE);        
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
    return h * equationConstant;
}