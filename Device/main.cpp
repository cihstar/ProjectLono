/***  Standard Includes   ***/
#include <string>
#include <sstream>

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
GSM* modules::gsm;
SDCard* modules::sdCard;
PressureSensor* modules::pressureSensor;

/*** Main Function - Initialise Everything! ***/
int main() {
    /* Device Info */
    string deviceId = "0";
    string serverUrl = "http://lono-rain.appspot.com";
    
    /* Initialise PC Serial Link */
    modules::pc = new PCSerial(USB_SERIAL_TX, USB_SERIAL_RX, 16);
    
    /* Print Info */
    util::printBreak();
    util::printInfo("Welcome to Project Lono - Smart Rain Gauge");
    util::printInfo("PC Serial Link initialised");
    
    /* Initialise SD Card */
    modules::sdCard = new SDCard(SD_MOSI, SD_MISO, SD_CLK, SD_CS);
    util::printInfo("SD Card initialised");
    
    /* Initialise GSM Module */
    util::printInfo("GSM Starting up...");
    modules::gsm = new GSM(GSM_TX, GSM_RTS, GSM_RX, GSM_CTS, GSM_RESET, GSM_TERM_ON);
    //configure server connection
    modules::gsm->configureServerConnection(serverUrl);    
    //register with server
    if (modules::gsm->httpPost("/reg","id:"+deviceId) == "Done")
    {
        util::printInfo("Successfully registered with server.");
        util::printInfo("Device ID = "+deviceId);
    }
    else
    {
        util::printError("Could not register with server.");
    }
    //get time from server
    string r = modules::gsm->httpGet("/time");
    util::printDebug(r);
    if (r.length() == 19)
    {
        util::setTime(r.substr(0,10),r.substr(11,8));
        util::printInfo("Successfully retrieved time from server");
    }
    else
    {
        util::printError("Could not retrieve time from server");
    }
    ///probably will later be done in wireless module?
    util::printInfo("GSM Module initialised");
    
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
    modules::pressureSensor = new PressureSensor(P_SENSE_OUT, P_SENSE_SLEEP);
    
    /* Finish Boot up Info Printing */
    util::printInfo("System Time -> " + util::getTimeStamp());
    util::printInfo("Project Lono Started Up!");
    util::printDebug("Debug Mode Enabled");  
    util::printInfo("Type help for list of commands");  
    util::printBreak();    
    
    /* Threads in various objects will now be running */
    modules::pc->setEnableInput(true);
    
    /* Start pressure sensor readings */
    modules::pressureSensor->setDimensions(0.015, 0.2, 0.001);
    modules::pressureSensor->calibrate(10514, 22629, 0.12);
    modules::pressureSensor->start(10000, 10, 100);
    
     modules::gsm->httpGet("/FROM_MAIN");
}