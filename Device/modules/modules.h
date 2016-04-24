/* File defines all of the modules used. 
    Include this module to have access to the objects in all files.
    Allows inter-module communication
*/

#ifndef __MODULES_H__
#define __MODULES_H__

#include "mbed.h"
#include "rtos.h"

#include "PCSerial.h"
#include "SDCard.h"
#include "PressureSensor.h"
#include "ui.h"
#include "BatteryLevel.h"
#include "GSM.h"
#include "XBEE.h"

#include "Wireless.h"

namespace modules
{
    extern PCSerial* pc;        
    extern SDCard* sdCard;  
    extern PressureSensor* pressureSensor;
    extern UI* ui;
    extern BatteryLevel* battery;
    extern GSM* gsm;
    extern XBEE* xbee;
}
#endif