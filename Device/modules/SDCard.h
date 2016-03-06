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
        SDCard(PinName a, PinName b, PinName c, PinName d);
        ~SDCard();
        void writeToLog(string s);
        void writeReading(Wireless::Reading r);
        
        Dimensions readDimensions();
        void writeDimensions(Dimensions d);
        
        Calibrate readCalibrateData();
        void writeCalibrateData(Calibrate c);
        
        bool fileExists(char *filename);
        
        string readLog();
        void resetLog();
    
    private:    
    bool active;
};

#endif