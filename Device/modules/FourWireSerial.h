#ifndef __FOUR_WIRE_SERIAL_H__
#define __FOUR_WIRE_SERIAL_H__

#include "mbed.h"
#include "rtos.h"
#include <string>
#include <memory>

#define FWS_MESSAGE_READY 0x02
#define FWS_BUFFER_LENGTH 1024

/* GSMMessage class for containing a message passed in the GSM threads */
class GSMMessage {
    private:
        string message[8];
        int length;
    public:
        GSMMessage(string m, int l);
        GSMMessage();
        int getLength();
        void upLength();
        void resetLength();
        string getMessage(int i=0);
        void setMessage(string m, int i);
        void addMessage(string m);
};

typedef std::unique_ptr<GSMMessage> ptr_GSM_msg;

/* Extension of the serial object to support an RTS and CTS line.
    However, never really used, as the GSM Module comms were reliable 
    with just tx and rx lines.
    
    Characters recieved are added to a circular buffer.
    When /r/n is recieved notifes the GSM recieve thread.
*/

class FourWireSerial {
   public:
    FourWireSerial(PinName rx, PinName cts, PinName tx, PinName rts);
    ~FourWireSerial();
    
    void sendData(string data);
    void sendByte(char byte);       
    void setBaud(int baud);    
    string printBuffer();
    void clearBuffer();
    void setRxThread(Thread* t, int* a);
    char getChar();
    
  private:
    DigitalIn CTS;
    DigitalOut RTS;   
    Serial serial;
    char charBuffer[FWS_BUFFER_LENGTH];    
    void recieveByte();    
    char lastC;
    bool messageStarted;        
    Thread* rxThread;
    bool setup;
    int* messagesAvailable;
    
    int readIndex;
    int writeIndex;
        
};

#endif