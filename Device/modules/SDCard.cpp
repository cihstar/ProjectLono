#include "SDCard.h"

SDCard::SDCard(PinName a, PinName b, PinName c, PinName d)
{   
    //add :sd(a,b,c,d,"sd")
}

SDCard::~SDCard(){}

void SDCard::writeToLog(string s)
{/*
    FILE *fp = fopen("/sd/log.txt", "w");
    if(fp == NULL) {
        error("Could not open file for write\n");
    }
    fprintf(fp, "%s\r\n", s);
    fclose(fp); */
}

void SDCard::writeReading(string s)
{/*
    FILE *fp = fopen("/sd/data.txt", "w");
    if(fp == NULL) {
        error("Could not open file for write\n");
    }
    fprintf(fp, "%s\r\n", s);
    fclose(fp); */
}
/*
Dimensions* SDCard::readDimensions()
{
    char line[128];
    FILE *fp = fopen("/sd/dimensions.txt","r");
    if (fp=NULL)
    {
        error("Could not read from dimensions file\n");
    }
    if (fgets(line,128,fp) == NULL)
    {
        error("unable to read dimensions\r\n");
    }
    
    Dimensions d(tube, funnel, out);
    return &d;
}

void SDCard::writeDimensions(Dimensions* d)
{
    FILE *fp = fopen("/sd/dimensions.txt","w");
    if (fp==NULL)
    {
        error("Could not open file for write\n");
    }
    fprintf(fp, "%f,%f,%f", d->getTubeRadius(), d->getFunnelRadius(), d->getOutTubeRadius());
    fclose();
}

Dimensions::Dimensions(float rTube, float rFunnel, float rOutTube) :
tubeRadius(rTube), funnelRadius(rFunnel), outTubeRadius(rOutTube)
{
}

Dimensions::~Dimensions(){}

void Dimensions::set(float rTube, float rFunnel, float rOutTube)
{
    tubeRadius = rTube;
    funnelRadius = rFunnel;
    rOutTube = rOutTube;
}
float Dimensions::getTubeRadius()
{
    return tubeRadius;
}

float Dimensions::getFunnelRadius()
{
    return funnelRadius;
}

float Dimensions::getOutTubeRadius()
{
    return outTubeRadius;
}*/