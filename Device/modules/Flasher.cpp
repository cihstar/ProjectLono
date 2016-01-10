#include "Flasher.h"

Flasher::Flasher(DigitalOut led) : 
x(500), led(led), active(true), flashThread(&Flasher::threadStarter,this,osPriorityNormal, 1024)
{
    flashThread.signal_set(1);
}

Flasher::~Flasher(){}

void Flasher::threadStarter(void const *p) {
  Flasher *instance = (Flasher*)p;
  instance->flashTask();
}

void Flasher::flashTask()
{
    while(true)
    {
        if (active)
        {
            led = !led;
        }
        else
        {
            led = 0;
        }
        Thread::wait(x);
    }
}

void Flasher::setRate(int r)
{
    x=r;
}

void Flasher::setActive(bool a)
{
    active = a;
}

void Flasher::toggle()
{
    active = !active;
}