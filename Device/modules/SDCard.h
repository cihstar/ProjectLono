#ifndef __SD_CARD_H__
#define __SD_CARD_H__

#include "mbed.h"
#include "SDFileSystem.h"
#include <string>
#include "Dimensions.h"

#include "Wireless.h"

class SDCard
{
    public:
        SDCard(PinName a, PinName b, PinName c, PinName d, bool act);
        ~SDCard();
        void writeToLog(string s);
        void writeReading(Wireless::Reading r);
        void clearReadings();
        
        Dimensions readDimensions();
        void writeDimensions(Dimensions d);
        
        Calibrate readCalibrateData();
        void writeCalibrateData(Calibrate c);
        
        Timing readTimingData();
        void writeTimingData(Timing t);        
        
        string readLog();
        void resetLog();
    
    private:    
    bool active;
};

#endif