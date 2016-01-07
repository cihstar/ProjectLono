/***  Standard Includes   ***/

/***     mbed Includes    ***/
#include "mbed.h"

/***   Project Includes   ***/
#include "GSM.h"

/*** Declarations ***/
DigitalOut myled(LED2);
DigitalOut myled1(LED3);
Serial pc(USBTX, USBRX);

/*** Main Function ***/
int main() {
    pc.printf("Loaded and ready to go!\r\n");
    
    GSM gsm;
    pc.printf("output %d", gsm.dostuff());
    while(1) {
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.6);
    }
}