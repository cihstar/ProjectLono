#ifndef __MODULES_H__
#define __MODULES_H__

#include "mbed.h"
#include "rtos.h"

#include "PCSerial.h"
#include "SDCard.h"
#include "PressureSensor.h"
#include "ui.h"
#include "BatteryLevel.h"
<<<<<<< HEAD
#include "GSM.h"
#include "XBEE.h"

#include "Wireless.h"
=======
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3

namespace modules
{
    extern PCSerial* pc;        
    extern SDCard* sdCard;  
    extern PressureSensor* pressureSensor;
    extern UI* ui;
    extern BatteryLevel* battery;
<<<<<<< HEAD
    extern GSM* gsm;
    extern XBEE* xbee;
=======
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
}
#endif