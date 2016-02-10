#include "GSM.h"
#include "util.h"

GSM::GSM(PinName tx, PinName cts, PinName rx, PinName rts, PinName termOn, PinName reset) : serial(rx,cts,tx,rts),
rxThread(&GSM::threadStarter, this, osPriorityNormal,1024), print(false),
txThread(&GSM::threadStarterTx, this, osPriorityNormal,1024),
termOn(termOn), reset(reset), respWaiting(0), respFront(0),connectedToServer(false),serverConfigured(false),
timeoutTimer(timerStarter, osTimerPeriodic, this), timeout(false), waitingForReply(false), replyFor(-1),
longOperationInProg(false)
{
    //Wait a few seconds for it to boot up
    Thread::wait(2000);    
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
    //Respond to messages from the GSM Module
    GSMMessage* m;
    string message;
    
    while(1)
    {
        m = serial.getNextMessage();
        message = m->getMessage(0);
        if (print)
        {
            util::print("GSM: " + message);
        }        
                
        
        if (waitingForReply)
        {
            mRespWaiting.lock();          
            sendCommandReplyQueue.put(m);
            replyFor++;
            replyFor %= respLength;
            respWaiting--;            
            mRespWaiting.unlock();
        }
        else
        {
            //unsolicited messages handled here.
            util::printInfo(message);
        }
    }
}

void GSM::txTask()
{
    while(1)
    {
        //util::printDebug("hello from tx task, waiting for reply = "+util::ToString(waitingForReply));
        Thread::wait(5);
        //send messages from queue
        if (!waitingForReply)
        {            
            osEvent e = sendQueue.get();
            if (e.status == osEventMessage)
            {
                GSMMessage* m = (GSMMessage*) e.value.p;
                serial.sendData(m->getMessage() + "\r\n");
                waitingForReply = true;
            }
        }
    }
}

void GSM::configureServerConnection(string url)
{
    util::printDebug("1");
    waitForLongOperationToFinish();
    util::printDebug("2");
    longOperationInProg = true;
    GSMMessage* m;
    m = sendCommand("AT^SICS=0,conType,GPSR0");
    if (m->getMessage(0) == "OK")
    {
        m = sendCommand("AT^SICS=0,apn,EEM2M");
        if (m->getMessage(0) == "OK")
        { 
            m = sendCommand("AT^SISS=0,srvType,http");
            if (m->getMessage(0) == "OK")
            {
                m = sendCommand("AT^SISS=0,conId,0");
                if (m->getMessage(0) == "OK")
                {
                    sendCommand("AT^SISS=0,address,"+url+":80");
                    util::printInfo("Server connection successfully configured to "+url);
                    connectedUrl = url;
                    serverConfigured = true;
                    longOperationInProg = false;
                    return;
                }
            }
        }
    }
    util::printError("Unable to configure server connection for URL = "+url);
    longOperationInProg = false;
}

bool GSM::connectToServer()
{
    if (serverConfigured)
    {
        connectedToServer = (((sendCommand("AT^SISO=0"))->getMessage(0)) == "OK");
        return connectedToServer;
    }
    else
    {
        return false;
    }
}

bool GSM::disconnectFromServer()
{
    if (connectedToServer && serverConfigured)
    {
    connectedToServer = !(((sendCommand("AT^SISC=0"))->getMessage(0)) == "OK");
    return connectedToServer;
    }
    else
    {
        return false;
    }
}

string GSM::httpPost(string url, string data)
{
    waitForLongOperationToFinish();
    longOperationInProg = true;
    connectToServer();
    if (!connectedToServer)
    {
        util::printError("HTTP Operation failed. Not connected to server");
        return "ERROR";
    }
    GSMMessage *m;
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
        util::printError("HTTP Operation failed. Not connected to server");
        return "ERROR";
    }
    GSMMessage *m;
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
        Thread::wait(10);
    }
}

GSMMessage* GSM::sendCommand(string c, int numResults)
{
    //Send the command to GSM, with added CR
    
    ///////////********* REMOVE THE /n when actually using!!!!!! //////////
    GSMMessage toSend(c,1);
    sendQueue.put(&toSend);
    
    //Wait for reply.
    mRespWaiting.lock();
    int me = respWaiting;
    me %= respLength;
    respWaiting+=numResults;
    mRespWaiting.unlock();
    int rxed = 0;
    
    GSMMessage* returnMessage = new GSMMessage();
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
            respWaiting-= (numResults - rxed);
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