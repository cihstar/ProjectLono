#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include <string>

namespace Wireless
{
    class Wireless
    {
        enum ConnectionType{NONE, GSM, XBEE};
        
        public:
            Wireless();
            ~Wireless();
            void setConnectionMode(ConnectionType t);
            ConnectionType getConnectionMode();
            void txString(string str);        
        private:
            ConnectionType mode;
            GSM* gsm;
            XBEE* xbee;
            Queue<string, 1024> txQueue;
            //send function adds to the queue
            
            //send thread:
            //if mode = none, leave in queue
            //if mode = xbee or gsm, send and wait for response. if ok remove from queue       
            
           /* void sendThread()
            {
                message m = txQueue.get();
                if (mode == None)
                {
                    txQueue.put(m);
                    return;
                }
                else (mode == GSM)
                {   
                    GSM::Status r = gsm.send(m);
                    if (r != GSM_SUCCESS)
                    {
                        txQueue.put(m)
                    }
                }
                else
                {                    
                    XBEE:Status r = xbee.send(m);
                    if (r != XBEE_SUCCESS)
                    {
                        txQueue.put(m)
                    }
                }
            }
            */
            
            /*GSM send:
            
            GSM::Status send(message m)
            {
                serial.send("Command to send data" + m);
                
                WAIT FOR RESPONSE:
                reply = rxQueue.get()
                
                if (reply == SUCCESS)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            } 
            
            and very similar for XBEE.
            
            The rxQueue is being populated by a similar function to PCSerial rx, listening for reply in
            interrupt and putting into queue when it is done. Or hmm maybe another thread will be needed
            to listen for unsolicited sendings? so maybe have a queue shared between the listening thread
            and the send function that will store the status and can be populated by the listen thread
            when the reply is found to be of the right type. Something like that.
            
            //in gsm/xbee, a thread and queue for responses.
            //so in the send function 
    };
}

#endif