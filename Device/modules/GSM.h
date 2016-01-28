#ifndef __GSM_H__
#define __GSM_H__

#include "FourWireSerial.h"

class GSM {
  public:
    GSM(PinName tx, PinName cts, PinName rx, PinName rts);
    ~GSM();
    void sendCommand(string c);
    void setPrint(bool p);
  private:
    FourWireSerial serial;
    static void threadStarter(void const *p);
    void rxTask();
    Thread rxThread;
    char buffer[1024];
    bool print;
};

#endif