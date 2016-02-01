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