#ifndef __P_SENSE_H__
#define __P_SENSE_H__

#include "mbed.h"
#include "rtos.h"

#include "Dimensions.h"

#include <string>

//Acceleration due to gravity in m.s^-2
#define G 9.81

class PressureSensor
{
    public:
        PressureSensor(PinName out, PinName sleep);
        ~PressureSensor();
        void start();
        void stopTimer();
        uint16_t read();
        void sleep();
        void wakeup();
        void setDimensions(Dimensions d);
        void calibrate(Calibrate c);
        void setTiming(int tTx, int tSamp, int reads);
        bool getActive();
        string getLastReading();
        int getTxInterval();
        
    private:
        AnalogIn sensor;
        DigitalOut sleepPin; 
        Thread timer;
        void timerTask();    
        static void timerStarter(void const *p);
        bool timerStarted;
        
        float toHeight( uint16_t adcVal);
        float calcTubeOut(float height);
        
        bool active;
        string lastReading;
        
        float reading;
        float area(float r);
        
        /* Reading variables */
        bool emptying;
        uint16_t samples;        
        float totalRain;
        float lasth;
        
        /* Timing - ms */
        int sampsPerTx;
        int sampInterval;
        int readsPerSamp;
        
        /* Physical Dimensions */
        float tubeArea;
        float outTubeArea;
        float funnelRatio; //mm
        
        /* Out tube on/off heights */
        float startEmptyHeight;
        float endEmptyHeight;
        
        /* ADC to height calibration */        
        float calibration;
        uint16_t offset;
};

#endif