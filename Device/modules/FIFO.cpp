#include "FIFO.h"
#include "rtos.h"
#include <string>

FIFO::FIFO() : front(0), length(0), s(1) {}
FIFO::~FIFO(){}

uint16_t FIFO::getLength()
{
    return length;
}

void FIFO::setLength(uint16_t x)
{
    length = x;
}

void FIFO::addByte(char byte)
{
    s.wait();   
    buffer[(front + length) % SIZE] = byte;
    length++;
    s.release();      
}

bool FIFO::contains(char c)
{
    for (int i = 0; i < SIZE; i++)
    {
        if (buffer[i] == c)
            return true;
    }
    return false;
}

char FIFO::getByte()
{
    s.wait();
    char byte = buffer[front];
    front++;
    length--;
    front %= SIZE;
    s.release();
    return byte;
}

void FIFO::empty()
{
    s.wait();
    for (int i = 0; i < SIZE; i++)
    {
        buffer[i] = '\0';
        length = 0;
        front = 0;
    }   
    s.release();
}

string FIFO::toString()
{
    if (length == 0)
    {
        return "";
    }
    s.wait();
    buffer[(front + length) %SIZE] = '\0';
    string str(buffer);
    empty();
    s.release();
    return str;   
}