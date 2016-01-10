#ifndef __FIFO_H__
#define __FIFO_H__

#include "mbed.h"
#include <string>
#include "rtos.h"

#define SIZE 1023

class FIFO{
    public:
        FIFO();
        ~FIFO();
        uint16_t getLength();
        void setLength(uint16_t x);
        void addByte(char byte);
        char getByte();
        void empty();
        bool contains(char c);
        string toString();
    private:
        uint16_t front;
        uint16_t length;
        char buffer[SIZE];
        Semaphore s;
};

#endif