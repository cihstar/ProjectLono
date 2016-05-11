/* Controls the Huawei MG323-B module
   Implements comms via UART
   HTTP Post and GET commands implemented
   Capability for SMS messages and calls also present
   
   AT Command specification: http://download-c.huawei.com/download/downloadCenter?downloadId=14254
*/

#include "GSM.h"
#include "util.h"

GSM::GSM(PinName tx, PinName cts, PinName rx, PinName rts, PinName ptermOn, PinName preset) : serial(rx,cts,tx,rts),
rxThread(&GSM::threadStarter, this, osPriorityNormal,2400), print(false),
txThread(&GSM::threadStarterTx, this, osPriorityNormal,412),
termOn(ptermOn), reset(preset), respWaiting(0), connectedToServer(false),serverConfigured(false),
timeoutTimer(timerStarter, osTimerPeriodic, this), timeout(false), waitingForReply(false), replyFor(-1),
longOperationInProg(false), powerOn(false), messagesAvailable(0)
{
    /* Tell serial where the rx thread is */
    serial.setRxThread(&rxThread, &messagesAvailable);    
}

GSM::~GSM()
{}

/* Start RX thred */
//https://developer.mbed.org/forum/mbed/topic/4388/
void GSM::threadStarter(void const *p)
{
    GSM *instance = (GSM*)p;
    instance->rxTask();
}

/* Start Tx Thread */
void GSM::threadStarterTx(void const *p)
{
    GSM *instance = (GSM*)p;
    instance->txTask();
}

/* Timeout timer for comms */
void GSM::timerStarter(void const* p)
{
    GSM *instance = (GSM*)p;
    instance->timeoutFunction();
}

/* Process incoming serial messages */
void GSM::rxTask()
{    
    bool messageComplete = false;
    char lastC = ' ';
    bool dontPrint = false;
    int j;
    int i;
    char c;
    while(1)
    {      
        /* wait for serial to say a message is available */
        Thread::signal_wait(FWS_MESSAGE_READY);  
        
        j = messagesAvailable;
        /* For each message waiting... */
        for (i = 0; i < j; i++)
        {
            messagesAvailable--;
            string message;
            
            messageComplete = false;
            
            while(!messageComplete)
            {
                /* Build message by taking chars out of circular buffer */
                c = serial.getChar();
                message.append(1,c);
                if (c == '\n' && lastC == '\r')
                {
                    messageComplete = true;
                }
                lastC = c;
            }
            
            /* Ignore empty messages */
            if (message != "" && message != "\r" && message != "\n" && message != "\r\n")
            {               
                if (print)
                {
                    /* Print the message to console */
                    util::print("GSM: " + message);
                }                        
            
                if (message.find("^SYSSTART") != string::npos)
                {
                    /* Powered on message */
                    util::printInfo("GSM Powered On");
                    powerOn = true;     
                    dontPrint = true;              
                }  
                
                if (message.find("closed the connection") != string::npos)
                {
                    /* Message saying server connection is closed */
                    util::printInfo("Server closed connection");
                    connectedToServer = false;
                }  
            
                if (waitingForReply)
                {
                    /* Give message to function which is waiting */
                    string* m = new string(message);
                    mRespWaiting.lock();          
                    sendCommandReplyQueue.put(m);                                        
                    replyFor++;
                    replyFor %= respLength;
                    respWaiting--;            
                    mRespWaiting.unlock();                    
                }
                else
                {
                    if (!print && !dontPrint)
                    {
                        dontPrint = false;
                        util::print("GSM Message: " + message);
                    }
                }
            }
        }   
    }
}

bool GSM::isOn()
{
    return powerOn;
}

void GSM::txTask()
{    
    GSMMessage* m;
    while(1)
    { 
        osEvent e = sendQueue.get();
        //send messages from queue
        if (!waitingForReply)
        {                        
            if (e.status == osEventMessage)
            {
                m = (GSMMessage*) e.value.p;
                serial.sendData(m->getMessage() + "\r");      
                if (print)
                {
                    util::printInfo("Sent: " + m->getMessage());
                }                               
                waitingForReply = true;                
            }
        }
    }
}

/* AT Commands to configure the GSM Module to connect to a URL via HTTP */
bool GSM::configureServerConnection(string url)
{    
    connectedToServer = false;     
    ptr_GSM_msg m;    
    m = sendCommand("AT^SICS=0,conType,GPRS0",1);    
    if (m->getMessage(0).find("OK") != string::npos)
    {        
        m = sendCommand("AT^SICS=0,apn,EEM2M",1);               
        if (m->getMessage(0).find("OK") != string::npos)
        {             
            m = sendCommand("AT^SISS=0,srvType,http",1);                       
            if (m->getMessage(0).find("OK") != string::npos)
            {                 
                m = sendCommand("AT^SISS=0,conId,0",1);                                
                if (m->getMessage(0).find("OK") != string::npos)
                {                                        
                    m = sendCommand("AT^SISS=0,address,"+url,1);                                     
                    if (m->getMessage(0).find("OK") != string::npos)
                    {
                        connectedUrl = url;
                        serverConfigured = true;                        
                        return true;
                    }                    
                }
            }
        }
    }
    longOperationInProg = false;
    return false;
}

/* Open the currently configured server connection */
bool GSM::connectToServer(string url)
{
    if (!connectedToServer || (connectedToServer && connectedUrl != url))
    {
        /* Configure Connection */        
        configureServerConnection(url);                 
        wait(3);        
        /* And connect to server */
        ptr_GSM_msg m;
        m = sendCommand("AT^SISO=0",2);      
        if (m->getMessage(0).find("OK") != string::npos)
        {
            /* Success! */         
            connectedToServer = true;            
            return true;
        }
        else
        {
            /* Fail */
            connectedToServer = false;
            return false;
        }
    }
    else
    {      
        //Already connected
        return true;
    }
}

/* Close connection */
bool GSM::disconnectFromServer()
{
    if (connectedToServer && serverConfigured)
    {
        connectedToServer = !(((sendCommand("AT^SISC=0"))->getMessage(0)).find("OK") != string::npos);
        return !connectedToServer;
    }
}

/* Send an HTTP Post request */
string GSM::httpPost(string url1, string url2, string data)
{
    waitForLongOperationToFinish();
    longOperationInProg = true;            
    if (!connectToServer(url1))
    {
        longOperationInProg = false;
        return "ERROR_NOT_CONNECTED";
    }  
            
    wait(2); 
    ptr_GSM_msg m;
    
    /* The command to send: */
    string str("AT^HTTPCMD=0,POST,"+url1+url2+","+util::ToString(data.length())+",\"application/x-www-form-urlencoded\"");   
    m = sendCommand(str);
    
    /* If successful, return connect. Then need to send the data to send */
    if (m->getMessage(0).find("CONNECT") != string::npos)
    {     
        m = sendCommand(data,3);
        if (m->getMessage(1).find("OK") != string::npos)
        {          
            /* Success */
            longOperationInProg = false;
            
            /* Return the reply from server */           
            return m->getMessage(0);
        }
    }    
    /* Failure */
    disconnectFromServer();
    longOperationInProg = false;
    return "ERROR_CONNECT_FAILED";
}

/* HTTP Get */
string GSM::httpGet(string url1, string url2)
{
    waitForLongOperationToFinish();
    longOperationInProg = true;
    if (!connectToServer(url1))
    {
        longOperationInProg = false;
        return "ERROR";
    }
    wait(2);
    ptr_GSM_msg m;
    m = sendCommand("AT^HTTPCMD=0,GET,"+url1+url2,3);
    if (m->getMessage(0) == "CONNECT")
    {
        if (m->getMessage(2) == "OK")
        {            
            disconnectFromServer();
            longOperationInProg = false;
            return m->getMessage(1);  
        }
    }    
    disconnectFromServer();
    longOperationInProg = false;
    return "ERROR";
}

void GSM::timeoutFunction()
{
    timeout = true;
}

void GSM::waitForLongOperationToFinish()
{
    while(longOperationInProg)
    {
       Thread::wait(1);
    }
}

/* The send command function. Waits for numResults number of reply messages */
ptr_GSM_msg GSM::sendCommand(string c, int numResults)
{
    /* Smart pointer object with message */
    ptr_GSM_msg returnMessage(new GSMMessage);   
    
    /* Exit if the module is off */
    if (!powerOn)
    {
        returnMessage->addMessage("PowerOff");
        return returnMessage;
    }
     
    /* Add message to send queue */
    GSMMessage toSend(c,1);
    sendQueue.put(&toSend);  
    
    /* Wait for reply */
    mRespWaiting.lock();
    int me = respWaiting;
    me %= respLength;
    respWaiting+=numResults;
    mRespWaiting.unlock();
    int rxed = 0;
    
    /* Start timeout */
    timeout = false;
    timeoutTimer.start(TIMEOUT*1000);
           
    /* Get reply messages. RX thread will set replyFor to our value and then we will get message from queue */
    while (rxed < numResults)
    {
        /* Timed out so exit */
        if (timeout)
        {
            for (int i = rxed; i < numResults; i++)
            {
                returnMessage->addMessage("TIMEOUT");
            }
            mRespWaiting.lock();
            respWaiting -= (numResults - rxed);
            mRespWaiting.unlock();

            break;
        }
        
        /* See if message is recieved for us */
        mRespWaiting.lock();
        if (replyFor == me)
        {        
            /* If it is, get from queue */    
            rxed++;
            me++;
            me%=respLength;            
            osEvent e = sendCommandReplyQueue.get();            
            if (e.status == osEventMessage)
            {                
                /* Add the message to our return message */
                string* m = ((string*) e.value.p);
                returnMessage->addMessage(*m);
                
                /* If an error on GSM module then rest of expected results will not be coming */
                if ((*m).find("ERROR") != string::npos)
                {
                    //Clear remaining requests because of error.
                    for (int i = rxed; i < numResults; i++)
                    {
                        returnMessage->addMessage("ERR_CLEARED");
                    }
                    mRespWaiting.lock();
                    respWaiting -= (numResults - rxed);
                    mRespWaiting.unlock();
                    rxed = numResults;
                }
                /* Delete receieved message */
                delete m;
            }                        
            timeout = false;            
            timeoutTimer.start(TIMEOUT * 1000);            
        }
        else
        {
            Thread::wait(10);
        }
        mRespWaiting.unlock();
    }
    
    /* Recieved all messages */
    mRespWaiting.lock();
    if (respWaiting == 0)
    {
        replyFor = -1;
    }
    waitingForReply = false;
    mRespWaiting.unlock();
    
    /* And send message back to function, unblocking */
    return returnMessage;
}

void GSM::setPrint(bool p)
{
    print = p;
}

/* Just send a message, get no reply */
void GSM::sendCommandNoReply(string cmd)
{
    if (powerOn)
    {
        serial.sendData(cmd+"\r");
    }                       
}