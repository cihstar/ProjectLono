#ifndef __MODULES_H__
#define __MODULES_H__

#include "mbed.h"
#include "rtos.h"

#include "GSM.h"
#include "PCSerial.h"
#include "Flasher.h"
#include "SDCard.h"
#include "PressureSensor.h"


namespace modules
{
    extern PCSerial* pc;
    extern GSM* gsm;
    extern Flasher* flasher[4];  
    extern SDCard* sdCard;  
    extern PressureSensor* pressureSensor;
}
#endif