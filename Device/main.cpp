/***  Standard Includes   ***/
#include <string>

/***     mbed Includes    ***/
#include "mbed.h"
#include "rtos.h"

/***   Project Includes   ***/
#include "pindef.h"
#include "util.h"
#include "modules.h"

//Module Declarations
Flasher* modules::flasher[4];
PCSerial* modules::pc;
//GSM gsm;

/*** Main Function - Initialise Everything! ***/
int main() { 
    /* Initialise PC Serial Link */
    modules::pc = new PCSerial(USB_SERIAL_TX, USB_SERIAL_RX, 16);
    
    /* Print Info */
    util::printBreak();
    util::printInfo("Welcome to Project Lono - Smart Rain Gauge");
    util::printInfo("PC Serial Link initialised");
    
    /* Init SD Card */
    
    /* Init LCD Screen and Buttons */
    
    /* Initialise Board LED Flasher Objects */
    modules::flasher[0] = new Flasher(BOARD_LED_1);
    modules::flasher[1] = new Flasher(BOARD_LED_2);
    modules::flasher[2] = new Flasher(BOARD_LED_3);
    modules::flasher[3] = new Flasher(BOARD_LED_4);  
    util::printInfo("Flashers initialised");  
    
    /* Init wireless (mode = none) */
    /* Set wireless mode */
    /* Then in GSM/Xbee constructor test the link, ask for reply */
    /* add to send queue to get current time from wireless link */
    
    /* Init Battery level sensor */
    
    /* Init Pressure Sensor */
    
    /* Finish Boot up Info Printing */
    util::printInfo("System Time -> " + util::getTimeStamp());
    util::printInfo("Project Lono Started Up!");
    util::printDebug("Debug Mode Enabled");  
    util::printInfo("Type help for list of commands");  
    util::printBreak();    
    
    /* Threads in various objects will now be running */
}