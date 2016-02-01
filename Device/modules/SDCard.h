#ifndef __SD_CARD_H__
#define __SD_CARD_H__

#include "mbed.h"
#include "SDFileSystem.h"
#include <string>

class SDCard
{
    public:
        SDCard(PinName a, PinName b, PinName c, PinName d);
        ~SDCard();
        void writeToLog(string s);
        void writeReading(string s);
    
    private:
      //  SDFileSystem sd;
};

#endif