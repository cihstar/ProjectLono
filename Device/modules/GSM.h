#ifndef __GSM_H__
#define __GSM_H__

#include "FourWireSerial.h"
#include "rtos.h"
#include "mbed.h"
#include <string>

#define respLength 8
#define TIMEOUT 15

class GSM {
  public:
    GSM(PinName tx, PinName cts, PinName rx, PinName rts, PinName reset, PinName termOn);
    ~GSM();
    GSMMessage* sendCommand(string c, int numResults=1);
    void setPrint(bool p);    
    string httpPost(string url, string data);
    string httpGet(string url);
    void configureServerConnection(string url);
    void timeoutFunction();
    
  private:
    FourWireSerial serial;
    DigitalOut reset;
    DigitalOut termOn;
    static void threadStarter(void const *p);
    static void threadStarterTx(void const *p);
    void rxTask();
    void txTask();
    Thread rxThread;
    Thread txThread;
    char buffer[1024];
    bool print;
    
    Queue<GSMMessage,respLength> sendQueue;
    bool waitingForReply;
    
    Queue<GSMMessage,respLength> sendCommandReplyQueue;
    int respWaiting;
    int respFront;
    int replyFor;
    Mutex mRespWaiting;
    bool respTo[respLength];
    
    string connectedUrl;
    
    bool connectToServer();
    bool disconnectFromServer();
    bool connectedToServer;
    bool serverConfigured;
        
    RtosTimer timeoutTimer;
    static void timerStarter(void const *p);
    bool timeout;

    bool longOperationInProg;
    void waitForLongOperationToFinish();
    
    
};

#endif