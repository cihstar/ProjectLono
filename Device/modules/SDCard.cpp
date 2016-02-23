#include "SDCard.h"
#include "util.h"

SDCard::SDCard(PinName a, PinName b, PinName c, PinName d)
{   
    active = false;
    //add :sd(a,b,c,d,"sd")
}

SDCard::~SDCard(){}

void SDCard::writeToLog(string s)
{
    if (active)
    {
        FILE *fp = fopen("/sd/log.txt", "w");
        if(fp == NULL) {
            util::printError("Could not open log file for write\n");
            return;
        }
        fprintf(fp, "%s\r\n", s);
        fclose(fp);
    }
}

void SDCard::writeReading(string s)
{
    if (active)
    {
        FILE *fp = fopen("/sd/data.txt", "w");
        if(fp == NULL) {
            util::printError("Could not open readings data file for write\n");
            return;
        }
        fprintf(fp, "%s\r\n", s);
        fclose(fp);
    }
}

Dimensions SDCard::readDimensions()
{
    Dimensions ret = {0,0,0,0,0};
    if (active)
    {
        char line[128];
        
        FILE *fp = fopen("/sd/dimensions.txt","r");
        if (fp=NULL)
        {
            util::printError("Could not read from dimensions file\n");
            return ret;
        }
        if (fgets(line,128,fp) == NULL)
        {
            util::printError("unable to read dimensions\r\n");
            return ret;
        }
        
        fclose(fp);
        
        string str(line);
        int nextComma = -1;
        float vals[5];
        
        
        for (int i = 0; i < 5; i++)
        {
            nextComma = str.find(",");        
            if (nextComma == -1)
            {
                util::printError("Error parsing dimensions string from sd card");
                return ret;
            }   
            
            vals[i] = std::atof((str.substr(0,str.length() - nextComma)).c_str());
            str = str.substr(nextComma);
            
        }
    
        
        Dimensions d= {vals[0], vals[1], vals[2], vals[3], vals[4] };
        return d;
    }
    return ret;
}

void SDCard::writeDimensions(Dimensions d)
{
    if (active)
    {
        FILE *fp = fopen("/sd/dimensions.txt","w");
        if (fp==NULL)
        {
            util::printError("Could not open file for write\n");
            return;
        }
        fprintf(fp, "%f,%f,%f,%f,%f,", d.tubeRadius, d.funnelRadius, d.outTubeRadius, d.outTubeWall, d.pressureSensorTubeRadius);
        fclose(fp);
    }
}

Calibrate SDCard::readCalibrateData()
{
    Calibrate ret = {0,0,0.0f};
    if (active)
    {
        char line[128];
        
        FILE *fp = fopen("/sd/calibrate.txt","r");
        if (fp=NULL)
        {
            util::printError("Could not read from calibration file\n");
            return ret;
        }
        if (fgets(line,128,fp) == NULL)
        {
            util::printError("unable to read calibration data fule\r\n");
            return ret;
        }
        
        fclose(fp);
        
        string str(line);
        int nextComma = -1;
        
        uint16_t vals[2];
        float full;
        
        for (int i = 0; i < 3; i++)
        {
            nextComma = str.find(",");        
            if (nextComma == -1)
            {
                util::printError("Error parsing calibration string from sd card");
                return ret;
            }   
            
            if (i == 2)
            {
                full = std::atof((str.substr(0,str.length() - nextComma)).c_str());
            }
            else
            {
                vals[i] = std::atof((str.substr(0,str.length() - nextComma)).c_str());
            }
            str = str.substr(nextComma);
            
        }
    
        
        Calibrate d= {vals[0], vals[1], vals[2]};
        return d;
    }
    return ret;
}

void SDCard::writeCalibrateData(Calibrate c)
{
    if (active)
    {
        FILE *fp = fopen("/sd/calibrate.txt","w");
        if (fp==NULL)
        {
            util::printError("Could not open file for write\n");
            return;
        }
        fprintf(fp, "%d,%d,%f,", c.fullAdc, c.emptyAdc, c.fullHeight);
        fclose(fp);
    }
}