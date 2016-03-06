#include "GSM.h"
#include "util.h"

GSM::GSM(PinName tx, PinName cts, PinName rx, PinName rts, PinName ptermOn, PinName preset) : serial(rx,cts,tx,rts),
rxThread(&GSM::threadStarter, this, osPriorityNormal,4096), print(true),
txThread(&GSM::threadStarterTx, this, osPriorityNormal,1024),
termOn(ptermOn), reset(preset), respWaiting(0), respFront(0),connectedToServer(false),serverConfigured(false),
timeoutTimer(timerStarter, osTimerPeriodic, this), timeout(false), waitingForReply(false), replyFor(-1),
longOperationInProg(false), powerOn(false), messagesAvailable(0)
{
    serial.setRxThread(&rxThread, &messagesAvailable);    
}

GSM::~GSM()
{}

void GSM::threadStarter(void const *p)
{
    GSM *instance = (GSM*)p;
    instance->rxTask();
}

void GSM::threadStarterTx(void const *p)
{
    GSM *instance = (GSM*)p;
    instance->txTask();
}

void GSM::timerStarter(void const* p)
{
    GSM *instance = (GSM*)p;
    instance->timeoutFunction();
}

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
      //  printf("\r\GSM RX Thread Used stack: %d\r\n", rxThread.used_stack());
        Thread::signal_wait(FWS_MESSAGE_READY);  
        j = messagesAvailable;
        for (i = 0; i < j; i++)
        {
            messagesAvailable--;
            string message;
            
            messageComplete = false;
            
            while(!messageComplete)
            {
                c = serial.getChar();
                if (c == '\n' && lastC == '\r')
                {
                    messageComplete = true;
                }
                else
                {
                    if (c == '\n')
                    {
                        message.append(1,'<');
                        message.append(1,'C');
                        message.append(1,'R');
                        message.append(1,'>');
                    }
                    if (c == '\n')
                    {
                        message.append(1,'<');
                        message.append(1,'L');
                        message.append(1,'F');
                        message.append(1,'>');
                    }
                    message.append(1,c);
                }
                lastC = c;
            }
            
            if (message != "" && message != "\r" && message != "\n" && message != "\r\n")
            {               
                if (print)
                {
                    util::print("GSM: " + message);
                }                        
            
                if (message.find("^SYSSTART") != string::npos)
                {
                    util::printInfo("GSM Powered On");
                    powerOn = true;     
                    dontPrint = true;              
                }    
            
                if (waitingForReply)
                {
                    GSMMessage m(message,1);
                    mRespWaiting.lock();          
                    sendCommandReplyQueue.put(&m);
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
                waitingForReply = true;                
            }
        }
    }
}

bool GSM::configureServerConnection(string url)
{    
    waitForLongOperationToFinish();    
    longOperationInProg = true;
    ptr_GSM_msg m;    
    m = sendCommand("AT^SICS=0,conType,GPRS0",1);    
    if (m->getMessage(0).find("OK") != string::npos)
    {        
        m = sendCommand("AT^SICS=0,apn,goto.virginmobile.uk",1);               
        if (m->getMessage(0).find("OK") != string::npos)
        {             
            m = sendCommand("AT^SISS=0,srvType,http",1);                       
            if (m->getMessage(0).find("OK") != string::npos)
            {                 
                m = sendCommand("AT^SISS=0,conId,0",1);                                
                if (m->getMessage(0).find("OK") != string::npos)
                {                                        
                    m = sendCommand("AT^SISS=0,address,"+url+":80",1);                                     
                    if (m->getMessage(0).find("OK") != string::npos)
                    {
                        connectedUrl = url;
                        serverConfigured = true;
                        longOperationInProg = false;
                        return true;
                    }                    
                }
            }
        }
    }
    longOperationInProg = false;
    return false;
}

bool GSM::connectToServer()
{
    if (serverConfigured)
    {
        ptr_GSM_msg m;
        m = sendCommand("AT^SISO=0",1);
        //util::printDebug("reply from connect: " + m->getMessage(0));
        if (m->getMessage(0).find("OK") != string::npos)
        {
            connectedToServer = true;
        }
        else
        {
            connectedToServer = false;
        }
        return connectedToServer;
    }
}

bool GSM::disconnectFromServer()
{
    if (connectedToServer && serverConfigured)
    {
        connectedToServer = !(((sendCommand("AT^SISC=0"))->getMessage(0)).find("OK") != string::npos);
        return !connectedToServer;
    }
}

string GSM::httpPost(string url, string data)
{
    waitForLongOperationToFinish();
    longOperationInProg = true;    
    if (!connectToServer())
    {
        longOperationInProg = false;  
        util::printDebug("not conn to server");      
        return "NOT_CONNECTED_TO_SERVER";
    }
    ptr_GSM_msg m;
    m = sendCommand("AT^HTTPCMD=0,POST,"+connectedUrl+url+","+util::ToString(data.length())+",'text/plain'");
    if (m->getMessage() == "CONNECT")
    {
        m = sendCommand(data,2);
        if (m->getMessage(1) == "OK")
        {
            disconnectFromServer();
            longOperationInProg = false;           
            return m->getMessage();
        }
    }    
    disconnectFromServer();
    longOperationInProg = false;
    return "ERROR";
}

string GSM::httpGet(string url)
{
    waitForLongOperationToFinish();
    longOperationInProg = true;
    connectToServer();
    if (!connectedToServer)
    {        
        longOperationInProg = false;
        return "ERROR";
    }
    ptr_GSM_msg m;
    m = sendCommand("AT^HTTPCMD=0,GET,"+connectedUrl+url,3);
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

ptr_GSM_msg GSM::sendCommand(string c, int numResults)
{
    ptr_GSM_msg returnMessage(new GSMMessage);   
    
    if (!powerOn)
    {
        returnMessage->addMessage("PowerOff");
        return returnMessage;
    }
    
  //  util::printInfo("message before making obj: " + c);
    GSMMessage toSend(c,1);
    sendQueue.put(&toSend);
  //  util::printInfo("Message in send command: " + toSend.getMessage(0));
    
    //Wait for reply.
    mRespWaiting.lock();
    int me = respWaiting;
    me %= respLength;
    respWaiting+=numResults;
    mRespWaiting.unlock();
    int rxed = 0;
    
    timeout = false;
    timeoutTimer.start(TIMEOUT*1000);
           
    while (rxed < numResults)
    {
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
        mRespWaiting.lock();
        if (replyFor == me)
        {            
            rxed++;
            me++;
            me%=respLength;            
            osEvent e = sendCommandReplyQueue.get();            
            if (e.status == osEventMessage)
            {                
                returnMessage->addMessage(((GSMMessage*) e.value.p)->getMessage());
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
    
    mRespWaiting.lock();
    if (respWaiting == 0)
    {
        replyFor = -1;
    }
    waitingForReply = false;
    mRespWaiting.unlock();
    
    return returnMessage;
}

void GSM::setPrint(bool p)
{
    print = p;
}

void GSM::sendCommandNoReply(string cmd)
{
    if (powerOn)
    {
        serial.sendData(cmd+"\r");
    }                       
}