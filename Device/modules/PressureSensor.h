#ifndef __P_SENSE_H__
#define __P_SENSE_H__

#include "mbed.h"
#include "rtos.h"

#include "Dimensions.h"
#include "Wireless.h"

#include <string>

//Acceleration due to gravity in m.s^-2
#define G 9.81

#define SAMPLE 0x01

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
        void setTiming(Timing t);
        bool getActive();
        string getLastReading();
        uint32_t getTxInterval();
        float toHeight( uint16_t adcVal);
        Wireless::Reading* getNextReading();        
        
    private:
        AnalogIn sensor;
        DigitalOut sleepPin; 
        Thread senseThread;
        void sensorTask();    
        static void threadStarter(void const *p);    
        
        RtosTimer timer;
        void timerTask();    
        static void timerStarter(void const *p);        
        
        bool active;
        string lastReading;
        
        float reading;
        float area(float r);
        
        Queue<Wireless::Reading,12> readingQueue;        
        
        /* Reading variables */
        bool emptying;
        uint32_t samples;        
        float totalRain;
        float lasth;
        
        /* Timing - ms */
        uint32_t sampsPerTx;
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