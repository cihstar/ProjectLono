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
#include "Dimensions.h"

/** Module Declarations **/
Flasher* modules::flasher[4];
PCSerial* modules::pc;
GSM* modules::gsm;
SDCard* modules::sdCard;
PressureSensor* modules::pressureSensor;
UI*  modules::ui;
BatteryLevel* modules::battery;

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
    
    /* Init LCD Screen and Buttons */
    modules::ui = new UI(LCD_SDA, LCD_SCL, LCD_RST, PB1, PB2, PB3);
    modules::ui->writeText("Project Lono Starting Up...");
    
    /* Initialise SD Card */
    modules::sdCard = new SDCard(SD_MOSI, SD_MISO, SD_CLK, SD_CS);
    util::printInfo("SD Card initialised");
    
    /* Initialise GSM Module */
    util::printInfo("GSM Starting up...");
    modules::gsm = new GSM(GSM_TX, GSM_RTS, GSM_RX, GSM_CTS, GSM_RESET, GSM_TERM_ON);
    modules::gsm->configureServerConnection(serverUrl);    // Configure server connection
    if (modules::gsm->httpPost("/reg","id:"+deviceId) == "Done") //register with server
    {
        util::printInfo("Successfully registered with server.");
        util::printInfo("Device ID = "+deviceId);
    }
    else
    {
        util::printError("Could not register with server.");
    }
    string r = modules::gsm->httpGet("/time"); //get time from server
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
    modules::battery = new BatteryLevel(BATTERY_LEVEL);
    util::printInfo("Battery Level: " + util::ToString(modules::battery->read()) + "V");
    
    /* Init Pressure Sensor */
    modules::pressureSensor = new PressureSensor(P_SENSE_OUT, P_SENSE_SLEEP);
    util::printInfo("Pressure Sensor Initialised");
    
    /* Finish Boot up Info Printing */
    util::printInfo("System Time -> " + util::getTimeStamp());
    util::printInfo("Project Lono Started Up!");
    util::printDebug("Debug Mode Enabled");  
    util::printInfo("Type help for list of commands");  
    util::printBreak();    
    
    /* Threads in various objects will now be running */
    modules::pc->setEnableInput(true);
    
    /* Start pressure sensor readings */
    Dimensions d = modules::sdCard->readDimensions();
    //0.015, 0.2, 0.001, 0.0015, 0.01
    modules::pressureSensor->setDimensions(d);
    //10514, 22629, 0.12
    Calibrate c = modules::sdCard->readCalibrateData();
    modules::pressureSensor->calibrate(c);
    modules::pressureSensor->setTiming(10000, 10, 100);
    modules::pressureSensor->start();
    
    /* Start Battery Level timer */
    modules::battery->startTimer(60000);
}