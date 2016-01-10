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
#include "PCSerial.h"
#include "GSM.h"
#include "pindef.h"
#include "FourWireSerial.h"
#include "util.h"

/*** Declarations ***/
DigitalOut myled(BOARD_LED_1);
DigitalOut myled1(BOARD_LED_2);
DigitalOut myled2(BOARD_LED_3);
DigitalOut myled3(BOARD_LED_4);

PCSerial pc(USB_SERIAL_TX, USB_SERIAL_RX, 16);
GSM gsm;

/* Thread Prototpyes */
void flashLed(void const *args);
void PCMessageRx(void const *args);

int flash;
/* Thread Declarations */
void flashLed(void const *args)
{
    myled2 = 1;
    while(true){
        Thread::wait(flash);
        myled2 = !myled2;
        //printDebug("Flash Thread");
    }
}

void PCMessageRx(void const *args)
{
    PCMessage* m;
    while(true)
    {        
        m = pc.getNextMessage();
        string mType = m->getMessageType();
        string ins = m->getInstruction();
        
        if (mType == "setRainMode")
        {
            printInfo("Setting rain mode to " + ins);
        }
        else if (mType == "setFlash")
        {
            printInfo("Set flash rate to "+ins+" ms");
            flash = atoi(ins.c_str());
        }
        else if (mType == "debug")
        {
            if (ins != "0")
            {
                printDebug("Enable Debug Mode");
                pc.setDebug(true);
            }
            else
            {
                printInfo("Disable Debug Mode");
                pc.setDebug(false);
            }
        }
        else
        {
            printError("Unknown Command '"+mType+"'");
        }
    }
}

/*** Main Function ***/
int main() {    
    flash = 500;     
    /* Thread to flash LED on board to show responsiveness */
    Thread flashThread(flashLed);
 
    /* Thread to Recieve message from PC serial and act on them */
    Thread PCRxMessageThread(PCMessageRx);
    
    printInfo("Project Lono Started Up!");
    printDebug("Debug Mode Enabled");    
    myled = 1; 
    while(1){}   
}