#ifndef __SD_CARD_H__
#define __SD_CARD_H__

#include "mbed.h"
#include "SDFileSystem.h"
#include <string>

class Dimensions
{
    public:
        Dimensions(float rTube, float rFunnel, float rOutTube);
        ~Dimensions();
        void set(float rTube, float rFunnel, float rOutTube);
        float getTubeRadius();
        float getFunnelRadius();
        float getOutTubeRadius();
    private:
        float tubeRadius;
        float funnelRadius;
        float outTubeRadius;
};

class SDCard
{
    public:
        SDCard(PinName a, PinName b, PinName c, PinName d);
        ~SDCard();
        void writeToLog(string s);
        void writeReading(string s);
        
        Dimensions readDimensions();
        void writeDimensions(Dimensions d);
    
    private:
      //  SDFileSystem sd;
};

#endif