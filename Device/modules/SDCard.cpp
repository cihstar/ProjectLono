#include "SDCard.h"
#include "util.h"

//AS PER: http://www.tayloredge.com/reference/Interface/SDPINOUT.pdf

SDFileSystem sd(p5, p6, p7, p8, "sd");

SDCard::SDCard(PinName a, PinName b, PinName c, PinName d):
active(false)
{      
    if (active)
    {     
        mkdir("/sd/lono", 0777);
        resetLog();
    }
}

SDCard::~SDCard(){}

bool SDCard::fileExists(char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

void SDCard::resetLog()
{
    if (active)
    {
        FILE *fp = fopen("/sd/lono/log.txt", "w");
        if(fp == NULL) {      
            fclose(fp);      
            util::printError("Could not open log file for write\n");
            return;
        }
        fprintf(fp, "New Log File");
        fclose(fp);
    }   
}

void SDCard::writeToLog(string s)
{
    if (active)
    {
        FILE *fp = fopen("/sd/lono/log.txt", "a");
        if(fp == NULL) {      
            fclose(fp);      
            util::printError("Could not open log file for write\n");
            return;
        }
        fprintf(fp, "%s\r\n",s.c_str());
        fclose(fp);
    }
}

string SDCard::readLog()
{
    if(active)
    {
        FILE *fp = fopen("/sd/lono/log.txt", "r");
        if (fp == NULL)
        {
            fclose(fp);
            util::printError("Could not open log file for read");
            return "ERROR";
        }
        if (!fp)
        {
            fclose(fp);
            util::printError("Log doesnot exist");
            return "ERROR";
        }
        char line[128];
        fgets(line, 128, fp);
        printf(line);
        fclose(fp);
        return string(line);
    }
}

void SDCard::writeReading(string s)
{
    if (active)
    {
        FILE *fp = fopen("/sd/lono/data.txt", "a");
        if(fp == NULL) {    
            fclose(fp);        
            util::printError("Could not open readings data file for write\n");
            return;
        }
        fprintf(fp, "%s\r\n", s.c_str());
        fclose(fp);
    }
}

Dimensions SDCard::readDimensions()
{        
    Dimensions ret = {0,0,0,0,0};
    if (active)
    {
        char line[128];        
        FILE *fp = fopen("/sd/lono/dimensions.txt","r");        
        if (fp == NULL)
        {       
            fclose(fp);     
            util::printError("Dimensions File Does not exist. Please create using 'setDimensions' command");
            return ret;
        }
        if (fgets(line,128,fp) == NULL)
        {
            fclose(fp);
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
            
            vals[i] = std::atof((str.substr(0,nextComma)).c_str());
            str = str.substr(nextComma+1);
            
        }
                    
        Dimensions d= {vals[0], vals[1], vals[2], vals[3], vals[4] };
        return d;
    }
    return ret;
}

void SDCard::writeDimensions(Dimensions d)
{
    util::printInfo("2");
    if (active)
    {
        util::printInfo("1");
        FILE *fp = fopen("/sd/lono/dimensions.txt","w");
        if (fp==NULL)
        {
            util::printError("Could not open file for write\n");
            fclose(fp);
            return;
        }
        util::printInfo("2");
        
        util::printInfo("Float 1:" + util::ToString(d.tubeRadius));
        
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
        
        FILE *fp = fopen("/sd/lono/calibrate.txt","r");
        if (fp == NULL)
        {
            fclose(fp);
            util::printError("Calibration File Does not exist. Please create using 'setCalibration' command");
            return ret;
        }
        if (fgets(line,128,fp) == NULL)
        {
            fclose(fp);
            util::printError("unable to read calibration data file\r\n");
            return ret;
        }
        
        fclose(fp);
        
        string str(line);
        int nextComma = -1;
        
        
        uint16_t vals[2];
        float full;
        string findStr;
        
        for (int i = 0; i < 3; i++)
        {            
            nextComma = str.find(",");        
            if (nextComma == -1)
            {
                util::printError("Error parsing calibration string from sd card");
                return ret;
            }   
            
            findStr = str.substr(0,nextComma);
            if (i == 2)
            {
                full = std::atof(findStr.c_str());                
            }
            else
            {
                vals[i] = std::atoi(findStr.c_str());                
            }
            str = str.substr(nextComma+1);            
        }
    
        
        Calibrate d= {vals[0], vals[1], full};
        return d;
    }
    return ret;
}

void SDCard::writeCalibrateData(Calibrate c)
{
    if (active)
    {
        FILE *fp = fopen("/sd/lono/calibrate.txt","w");
        if (fp==NULL)
        {            
            fclose(fp);
            util::printError("Could not open file for write\n");
            return;
        }
        fprintf(fp, "%d,%d,%f,", c.fullAdc, c.emptyAdc, c.fullHeight);
        fclose(fp);
    }
}