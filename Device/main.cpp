/***************************************************
 Project Lono - 3rd Year Project
 Smart, Autonomous, Connected Rain Gauge System
 ---------------------------------------------------
 Chris Holbrow
 cih1g13@soton.ac.uk
 ---------------------------------------------------
 Main File
 
 ***************************************************/

/***  Standard Includes   ***/
#include <string>

/***     mbed Includes    ***/
#include "mbed.h"
#include "rtos.h"

/***   Project Includes   ***/
#include "pindef.h"
#include "util.h"
#include "modules.h"

/*** Module Declarations ***/

//Board LEDS
Flasher* modules::flasher[4];
PCSerial* modules::pc;
//GSM gsm;

/*** Main Function ***/
int main() { 
    modules::flasher[0] = new Flasher(BOARD_LED_1);
    modules::flasher[1] = new Flasher(BOARD_LED_2);
    modules::flasher[2] = new Flasher(BOARD_LED_3);
    modules::flasher[3] = new Flasher(BOARD_LED_4);    
    modules::pc = new PCSerial(USB_SERIAL_TX, USB_SERIAL_RX, 16);
    /* Print Startup Info */
    util::printBreak();
    util::printInfo("Project Lono Started Up!");
    util::printDebug("Debug Mode Enabled");    
    util::printBreak();

    
    while(1){}   
}