#ifndef __FLASHER_H__
#define __FLASHER_H__

#include "mbed.h"
#include "rtos.h"

class Flasher
{
    public:
        Flasher(DigitalOut led);
        ~Flasher();
        void setRate(int r);
        void setActive(bool a);
        void toggle();
    private:
        int x;
        bool active;
        DigitalOut led;
        Thread flashThread;     
        static void threadStarter(void const *p);
        void flashTask();    
};

#endif