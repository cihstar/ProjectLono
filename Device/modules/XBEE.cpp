#include "XBEE.h"
#include "util.h"

XBEE::XBEE(PinName tx, PinName rx): xbee(tx,rx), read(0), write(0), msgAvailable(0),
timeoutTimer(timerStarter, osTimerPeriodic, this), timeout(false)
{
    xbee.attach(this, &XBEE::rxByte);
}

void XBEE::timerStarter(void const *p)
{
    XBEE *instance = (XBEE*)p;
    instance->timeoutTask();
}

void XBEE::timeoutTask()
{    
    timeout = true;
}

void XBEE::send(string str)
{
    for (int i = 0; i < str.length(); i++)
    {
        xbee.putc(str[i]);
    }   
}

void XBEE::rxByte()
{
    /* Get rxed char from xbee */
    char c = xbee.getc();
    
    if (c == '\r')
    {
        /* XBEE ends messages with \r */
        msgAvailable++;   
    }
    else
    {
        /* Add msg contents to buffer */
        buffer[write] = c;   
        write++;
        write %= LENGTH;
    }
}

char XBEE::getChar()
{
    char c =  buffer[read];
    read++;
    read %= LENGTH;
    return c;
}

string XBEE::getReply()
{
    timeoutTimer.start(10000);
    timeout = false;
    
    string reply;
    
    while(1)
    {
        if (timeout)
        {
            return "TIMEOUT";
        }      
        if (msgAvailable > 0)
        {
            char c = getChar();
            if (c == '\r')
            {
                /* End of message */
                msgAvailable--;
                return reply;
            }
            else
            {
                reply.append(1,c);
            }
        }
        Thread::wait(10);
    }
}


