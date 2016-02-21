#include "PCSerial.h"
#include "modules.h"
#include "util.h"
#include "Dimensions.h"

void PCSerial::rxTask()
{
    rxThread.signal_wait(1);    
    PCMessage* m;
    string mType;
    string mIns[M];
    while(true)
    {             
        m = modules::pc->getNextMessage();
        mType = m->getMessageType();
        for (int i = 0; i < M; i++)
        {
            mIns[i] = m->getInstruction(i);
        }

        modules::sdCard->writeToLog("PC: "+mType+" "+mIns[0]+" "+mIns[1]+" "+mIns[2]);
         
        if (gsmMode)
        {
            if (mType == "directMode")
            {
                util::printBreak();
                util::print("Direct Mode OFF");
                modules::gsm->setPrint(false);
                gsmMode = false;    
            }
            else
            {
                modules::gsm->sendCommandNoReply(mType);
            }
        }  
        else
        {
            if (mType == "help")
            {
                util::printInfo("todo: Write Help.");
            }
            else if (mType == "setRainMode")
            {
                util::printInfo("Setting rain mode to " + mIns[0]);
            }
            else if (mType == "flash")
            {
                int i = util::ToInt(mIns[0]);
                if (i < 4)
                {
                    util::printInfo("Toggling flasher "+mIns[0]);
                    modules::flasher[i]->toggle();
                }
                else
                {
                    util::printError("No flasher of that number");  
                }
            }
            else if (mType == "flashRate")
            {
                int i = util::ToInt(mIns[0]);
                int r = util::ToInt(mIns[1]);
                if (i < 4)
                {
                    if (r >= 0)
                    {
                        util::printInfo("Setting rate of flasher "+mIns[0]+" to "+mIns[1]+" ms");
                        modules::flasher[i]->setRate(r);
                    }
                    else
                    {
                        util::printError("Specify value for flash rate");
                    }
                }
                else
                {
                    util::printError("No flasher of that number");
                }
            }
            else if (mType == "debug")
            {
                if (mIns[0] != "0")
                {
                    util::printDebug("Enable Debug Mode");
                    modules::pc->setDebug(true);
                }
                else
                {
                    util::printInfo("Disable Debug Mode");
                    modules::pc->setDebug(false);
                }
            }
            else if (mType == "getTime")
            {
                util::printInfo("System Time -> "+util::getTimeStamp());
            }
            else if (mType == "setTime")
            {
                if (util::setTime(mIns[0], mIns[1]))
                {               
                    util::printInfo("Time Set Successfully. Current System Time: "+util::getTimeStamp());
                }
                else
                {
                    util::printError("Incorrect Time/Date input. Use mm/dd/yyyy hh:mm:ss");
                }
            }
            else if (mType == "directMode")
            {
                if (mIns[0] == "GSM")
                {
                util::print("Now communicating direct to GSM Module. Type 'directMode' to deactivate");
                util::printBreak();
                modules::gsm->setPrint(true);
                gsmMode = true;
                }
                else
                {
                    util::printError("Please choose component for direct comms mode");
                }
            }
            else if (mType == "readPressure")
            {
                util::printInfo(util::ToString(modules::pressureSensor->read()));
            }
            else if (mType == "setDimensions")
            {
                modules::pressureSensor->stopTimer();
                Dimensions d = {util::ToFloat(mIns[0]), util::ToFloat(mIns[1]), util::ToFloat(mIns[2]), util::ToFloat(mIns[3]), util::ToFloat(mIns[4])};
                modules::sdCard->writeDimensions(d);
                modules::pressureSensor->setDimensions(d);  
                modules::pressureSensor->start(); 
            }
            else if (mType == "setCalibration")
            {
                modules::pressureSensor->stopTimer();
                Calibrate c = {util::ToUint(mIns[0]), util::ToUint(mIns[1]), util::ToFloat(mIns[2])};
                modules::sdCard->writeCalibrateData(c);
                modules::pressureSensor->calibrate(c);
                modules::pressureSensor->start();
            }
            else
            {
                util::printError("Unknown Command '"+mType+"'");
            }
        }
    }
}
