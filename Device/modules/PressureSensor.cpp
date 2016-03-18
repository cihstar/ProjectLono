#include "PressureSensor.h"
#include "modules.h"
#include "util.h"
#include "Wireless.h"
#include <math.h>

PressureSensor::PressureSensor(PinName out, PinName sleep) :
sensor(out), sleepPin(sleep), senseThread(&PressureSensor::threadStarter, this, osPriorityNormal,2548), samples(0),
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
    equationConstant = (sqrt(2 * pow(outTubeArea,2) * G) / tubeArea) * (sampInterval);
    equationConstant /= 1000;    
    
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
    util::printInfo("Equation Constant: " + util::ToString(equationConstant));
    util::printInfo("Self emptying starts at " + util::ToString(startEmptyHeight) +"m and ends at " + util::ToString(endEmptyHeight) + "m");
    
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
    float outTubeWallArea = area(d.outTubeRadius + d.outTubeWall) - outTubeArea;
    tubeArea = area(d.tubeRadius) - outTubeWallArea - area(d.pressureSensorTubeRadius);
    funnelRatio = tubeArea / (M_PI * pow(d.funnelRadius,2));
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
        adcRead += (uint32_t) (sensor.read_u16() >> 8); //make 8 bit 
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
   float deltah;
   string str;
   string time;  
   
   int adc;
   
   float firstNegativeh;
   int firstNegativeAdc;
   float firsth = 0;
   
   int negativeDeltas = 0;    
   int positiveDeltas = 0;
   int zerodeltas = 0;
   
   bool emptyStoppedBeforeBottom = false;
    
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
        adc = read();
        h = toHeight(adc);       
        
        /* Constrain height to be > 0 */
        if (h < 0)
        {
            h = 0;
        }
        
        deltah = h - lasth;        
        
        /* Keep track of consecutive falls in level */
        if (deltah < 0)
        {
            if (negativeDeltas == 0)
            {
                firstNegativeh = h;
                firstNegativeAdc = adc;
            }
           negativeDeltas++;
           positiveDeltas = 0;
           zerodeltas = 0;
        }
        
        
        else if (deltah > 0)
        {
            positiveDeltas++;
            negativeDeltas = 0;
            zerodeltas = 0;
        }
        
        else
        {
            zerodeltas++;
            negativeDeltas = 0;
            positiveDeltas = 0;
        }
        
        //util::printDebug("0: "+ util::ToString(zerodeltas)+"  -: "+util::ToString(negativeDeltas)+"  +: "+util::ToString(positiveDeltas));
        
        /* Save */
        lasth = h;        
        
        
        /* Check if tube will be emptying or not */
        if (emptying)
        {
            if ( positiveDeltas == 5 || zerodeltas == 5 )
            {                                                             
                emptying = false;
                if (h > endEmptyHeight)
                {
                    util::printDebug("Tube Stopped Emptying not at bottom");   
                    emptyStoppedBeforeBottom = true;
                }
                else
                {
                    util::printDebug("Tube Stopped Emptying");   
                    emptyStoppedBeforeBottom = false;
                }
            }
        }
        else
        {
            if (negativeDeltas == 3)
            {                       
                //5 consecutive -ve deltas, probably emptying                    
                emptying = true;
                
                util::printDebug("Tube Emptying");
                
                /* And recalibrate? */
                if (!emptyStoppedBeforeBottom)
                {
                    if ((firstNegativeh < (startEmptyHeight-0.002) || (firstNegativeh > (startEmptyHeight+0.002))))
                    {
                        //calibration = startEmptyHeight / (firstNegativeAdc - offset);
                        util::printDebug("recal: thinks h is " + util::ToString(firstNegativeh) + " fullADC now = "+util::ToString(firstNegativeAdc));
                        samples = 0;
                        totalRain = 0;
                    }
                }
            }
        }       
        
        /* Caclulate total rain in this sample and accumulate */
        /* Ignore if emptying */
        if(!emptying)
        {         
            if (samples == 0) 
            {
                firsth = h;
            }              
                
            samples++;
        
            /* Time to tx sample */
            if (samples == sampsPerTx)
            {
                /* Last sample */
                totalRain = h - firsth;
                       
                /* Stop timer while processing */             
                timer.stop();                
                /* Convert height in tube to rainfall in mm */
                reading = (totalRain * funnelRatio) * 1000;  
                if (reading < 0)
                {
                    reading = 0;
                }                        
                str = util::ToString(reading);                          
                lastReading = str;
                
                /* Send reading */
                Wireless::Reading r = {reading, getTxInterval(), util::getTimeStamp()};
                readingQueue.put(&r);
                wait(1);
                
                /* Reset for next reading */
                samples = 0;              
                totalRain = 0;
                timer.start(sampInterval);                                               
            }
        }
        
        /* Turn off 5V regulator */
        sleep();
        
        /* Wait for timer signal to smaple again */
        Thread::signal_wait(SAMPLE);        
    }
}

Wireless::Reading* PressureSensor::getNextReading()
{
    osEvent e = readingQueue.get(10);
    if (e.status == osEventMessage)
    {
        return (Wireless::Reading*)e.value.p;        
    }   
    return NULL;
}

string PressureSensor::getLastReading()
{
    return lastReading;
}

float PressureSensor::calcTubeOut(float h)
{
    return h * equationConstant;
}