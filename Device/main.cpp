/*  Project Lono - Rain Gauge
    -------------------------
    main.cpp
    
    Chris Holbrow - cih1g13@soton.ac.uk
    
    Initialise device modules and run threads. 
    --------------------------
    Notes: 
        - To compile run make in the Device directory.
        - Add any extra source files to the objects list in the make file
        - Requires the arm-none-eabi-g++ compiler 
        - Make file copies to the device - may need to check drive letter
*/           

/***  Standard Includes   ***/
#include <string>
#include <sstream>

/***     mbed Includes    ***/
#include "mbed.h"
#include "rtos.h"
#include "PowerControl.h"
#include "EthernetPowerControl.h"

/***   Project Includes   ***/
#include "pindef.h"
#include "util.h"
#include "modules.h"
#include "Dimensions.h"
#include "Commands.h"

/* Initialise SD Card */
SDFileSystem sd(p5, p6, p7, p8, "sd");

/* Watchdog Timer - reset device on software fault */
/* https://developer.mbed.org/cookbook/WatchDog-Timer */
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

/* Run timer to kick the watchdog */
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

#define WATCHDOG

/*** Main Function - Initialise Everything! ***/
int main() {  
       
    #ifdef WATCHDOG
        /* Watchdog Setup */
        RtosTimer watchdogTimer(timerTask, osTimerPeriodic);
        wdt.kick(10.0); //set to 10 seconds
        watchdogTimer.start(8000);           
    #endif
    
    /* Power Down Ethernet */
    PHY_PowerDown();
    
    /* Initialise PC Serial Link */
    modules::pc = new PCSerial(USB_SERIAL_TX, USB_SERIAL_RX, 128);
   // modules::pc = new PCSerial(XBEE_TX, XBEE_RX, 128);
    buildCommandList();
    
    /* Init SD Card */
    modules::sdCard = new SDCard(SD_MOSI, SD_MISO, SD_CLK, SD_CS, true);
    
    /* Print Info */
    util::printBreak();
    util::printInfo("Welcome to Project Lono - Smart Rain Gauge");
    util::printInfo("PC Serial Link initialised");
    util::printInfo("SD Card initialised");
    
    /* Init LCD Screen and Buttons */
    modules::ui = new UI(LCD_SDA, LCD_SCL, LCD_RST, PB1, PB2, PB3);
    modules::ui->writeText("Project Lono", "Initialising...");   
    util::printInfo("LCD Screen Initialised");    
    
    /* Initialise Wireless Module */  
    Wireless::init();     
    modules::gsm = new GSM(GSM_TX, GSM_RTS, GSM_RX, GSM_CTS, GSM_RESET, GSM_TERM_ON);
    modules::xbee = new XBEE(XBEE_TX, XBEE_RX);         
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
    
    /* Get physical dimensions from SD card */  
    Dimensions d = modules::sdCard->readDimensions();
    modules::pressureSensor->setDimensions(d);  

    /* Get calibration data from SD Card */
    Calibrate c = modules::sdCard->readCalibrateData();
    modules::pressureSensor->calibrate(c);
        
    /* Get timing data from SD card */
    Timing t = modules::sdCard->readTimingData();
    modules::pressureSensor->setTiming(t);    
    
    /* Start sensor sampling */
    modules::pressureSensor->start();    
    
    /* Finish Boot up Info Printing */
    util::printInfo("System Time -> " + util::getTimeStamp());
    util::printInfo("Project Lono Started Up!");
    util::printDebug("Debug Mode Enabled");  
    util::printInfo("Type help for list of commands");  
    util::printBreak();
    
    /* Show LCD Menu (not complete yet) */
    modules::ui->showMenu();  
      
    /* Setup now complete. Run main loop tasks */
    /* PC Messages and Wirless module work done alternately in
       main loop as had issues with memory overflow running them
       in seperate threads */  
    while(1)
    {
        /* Process incoming messages from the PC */
        modules::pc->rxTask();
        
        /* Transmit rainfall readings via selected mode */
        Wireless::sendReadings();
        
        /* Wait */
        Thread::wait(10);
    }
}
