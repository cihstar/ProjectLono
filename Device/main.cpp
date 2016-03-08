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
#include "Commands.h"

SDFileSystem sd(p5, p6, p7, p8, "sd");

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

//#define WATCHDOG

/*** Main Function - Initialise Everything! ***/
int main() {  
       
    bool startedFromWatchdog = false;
    #ifdef WATCHDOG
        /* Watchdog Setup */
        RtosTimer watchdogTimer(timerTask, osTimerPeriodic);
        wdt.kick(10.0); //set to 10 seconds
        watchdogTimer.start(8000);        
        if ((LPC_WDT->WDMOD >> 2) & 1)
        {
            startedFromWatchdog = true;
        }     
    #endif
    /* Initialise PC Serial Link */
    modules::pc = new PCSerial(USB_SERIAL_TX, USB_SERIAL_RX, 128);
    buildCommandList();
    
    /* Init SD Card */
    modules::sdCard = new SDCard(SD_MOSI, SD_MISO, SD_CLK, SD_CS);
    
    /* Print Info */
    util::printBreak();
    util::printInfo("Welcome to Project Lono - Smart Rain Gauge");
    util::printInfo("PC Serial Link initialised");
    util::printInfo("SD Card initialised");
    
    if (startedFromWatchdog)
    {
        util::printInfo("Restarted after crash by watchdog!");
    }
    
    /* Init LCD Screen and Buttons */
    modules::ui = new UI(LCD_SDA, LCD_SCL, LCD_RST, PB1, PB2, PB3);
    modules::ui->writeText("Project Lono", "Initialising...");   
    util::printInfo("LCD Screen Initialised");    
    
    /* Initialise Wireless Module */  
    Wireless::init();     
    modules::gsm = new GSM(GSM_TX, GSM_RTS, GSM_RX, GSM_CTS, GSM_RESET, GSM_TERM_ON);
    modules::xbee = new XBEE();     
    Wireless::setConnectionMode(Wireless::GSM);
    util::printInfo("Wirelesss Module initialised");                   
       
    /* Init Battery level sensor */
    modules::battery = new BatteryLevel(BATTERY_LEVEL);
    util::printInfo("Battery Level: " + util::ToString(modules::battery->read()) + "V");    
    
    /* Init Pressure Sensor */
    modules::pressureSensor = new PressureSensor(P_SENSE_OUT, P_SENSE_SLEEP);
    util::printInfo("Pressure Sensor Initialised");    
        
    /* Threads in various objects will now be running */            
    
    /* Enable PC input */
    modules::pc->setEnableInput(true);
    
    /* Start pressure sensor readings */    
    Dimensions d = modules::sdCard->readDimensions();
    modules::pressureSensor->setDimensions(d);  
     
    Calibrate c = modules::sdCard->readCalibrateData();
    modules::pressureSensor->calibrate(c);
        
    Timing t = modules::sdCard->readTimingData();
    modules::pressureSensor->setTiming(t);    
    
    modules::pressureSensor->start();    
    
    /* Finish Boot up Info Printing */
    util::printInfo("System Time -> " + util::getTimeStamp());
    util::printInfo("Project Lono Started Up!");
    util::printDebug("Debug Mode Enabled");  
    util::printInfo("Type help for list of commands");  
    util::printBreak();
    
    /* And run UI LCD/Buttons in Main Thread */
    modules::ui->showMenu();  
    
     /* Setup is now done. Run sendReadings() to process readings and tx them as they arrive
       while(1) so this loop should never exit. */   
    Wireless::sendReadings();
}
