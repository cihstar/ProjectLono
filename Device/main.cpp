/***************************************************
 Project Lono - 3rd Year Project
 Smart, Autonomous, Connected Rain Gauge System
 ---------------------------------------------------
 Chris Holbrow
 cih1g13@soton.ac.uk
 ---------------------------------------------------
 Main File
 
 Wrap Debug code in D().
 #define DEBUG in Makefile will comile the debug code
 
 ***************************************************/

/***  Standard Includes   ***/

/***     mbed Includes    ***/
#include "mbed.h"

/***   Project Includes   ***/
#include "pcPrint.h"
#include "GSM.h"
#include "pindef.h"

/*** Declarations ***/
DigitalOut myled(BOARD_LED_1);
DigitalOut myled1(BOARD_LED_2);
DigitalOut myled2(BOARD_LED_3);
DigitalOut myled3(BOARD_LED_4);

/*** Main Function ***/
int main() {     
    pc.printf("\nProject Lono\n");
    D(pc.printf("Debug Mode Active\n"));
    GSM gsm;
    D(pc.printf("yo there %d\n",gsm.dostuff()));
    while(1) {
       D(myled = 1;
        myled1 = 1;
        myled2 = 1;
        myled3 = 1;
        wait(0.2);
        myled = 0;
        myled1 = 0;
        myled2 = 0;
        myled3 = 0;
        wait(0.7);)
    }
}