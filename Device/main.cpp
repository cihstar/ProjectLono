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
#include "Wireless.h"

class Watchdog {
public:
// Load timeout value in watchdog timer and enable
    void kick(float s) {
        LPC_WDT->WDCLKSEL = 0x1;                // Set CLK src to PCLK
        uint32_t clk = SystemCoreClock / 16;    // WD has a fixed /4 prescaler, PCLK default is /4
        LPC_WDT->WDTC = s * (float)clk;
        LPC_WDT->WDMOD = 0x3;                   // Enabled and Reset
        kick();
    }
// "kick" or "feed" the dog - reset the watchdog timer
// by writing this required bit pattern
    void kick() {
        LPC_WDT->WDFEED = 0xAA;
        LPC_WDT->WDFEED = 0x55;
    }
};

Watchdog wdt;
void timerTask(void const *p)
{    
    wdt.kick();
}

/** Module Declarations **/
PCSerial* modules::pc;
SDCard* modules::sdCard;
PressureSensor* modules::pressureSensor;
UI*  modules::ui;
BatteryLevel* modules::battery;
GSM* modules::gsm;
XBEE* modules::xbee;

/*** Main Function - Initialise Everything! ***/
int main() {
    /* Watchdog Setup */
    RtosTimer watchdogTimer(timerTask, osTimerPeriodic);
    wdt.kick(10.0); //set to 10 seconds
    watchdogTimer.start(8000);
    bool startedFromWatchdog = false;
    if ((LPC_WDT->WDMOD >> 2) & 1)
    {
        startedFromWatchdog = true;
    } 
    
    /* Initialise PC Serial Link */
    modules::pc = new PCSerial(USB_SERIAL_TX, USB_SERIAL_RX, 128);
    
    /* Print Info */
    util::printBreak();
    util::printInfo("Welcome to Project Lono - Smart Rain Gauge");
    util::printInfo("PC Serial Link initialised");
    
    if (startedFromWatchdog)
    {
        util::printInfo("Restarted after crash by watchdog!");
    }
    
    /* Init LCD Screen and Buttons */
    modules::ui = new UI(LCD_SDA, LCD_SCL, LCD_RST, PB1, PB2, PB3);
    modules::ui->writeText("Project Lono Starting Up...");
    util::printInfo("LCD Screen Initialised");
    
    /* Initialise SD Card */
    modules::sdCard = new SDCard(SD_MOSI, SD_MISO, SD_CLK, SD_CS);
    util::printInfo("SD Card initialised");
    
    /* Initialise Wireless Module */
    Wireless::init();    
    modules::gsm = new GSM(GSM_TX, GSM_RTS, GSM_RX, GSM_CTS, GSM_RESET, GSM_TERM_ON);
    modules::xbee = new XBEE();
    Wireless::setConnectionMode(Wireless::NONE);
           
    util::printInfo("Wirelesss Module initialised");                
       
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
    
    /* And goodbye */
    Thread::wait(osWaitForever);
}