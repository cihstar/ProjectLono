#include "GSM.h"
#include "util.h"

GSM::GSM(PinName tx, PinName cts, PinName rx, PinName rts, PinName termOn, PinName reset) : serial(rx,cts,tx,rts),
rxThread(&GSM::threadStarter, this, osPriorityNormal,1024), print(false),
termOn(termOn), reset(reset)
{    
}

GSM::~GSM()
{}

void GSM::threadStarter(void const *p) {
  GSM *instance = (GSM*)p;
  instance->rxTask();
}

void GSM::rxTask()
{
    //Respond to messages from the GSM Module
    GSMMessage* m;
    string message;
    
    while(1)
    {
        m = serial.getNextMessage();
        message = m->getMessage();
        if (print)
        {
            util::print("GSM: " + message);
        }
        
        //Handle responses to return data from GSM hereee
    }
}

void GSM::sendCommand(string c)
{
    serial.sendData(c+'\r');
}

void GSM::setPrint(bool p)
{
    print = p;
}