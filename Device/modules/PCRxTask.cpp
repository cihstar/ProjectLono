#include "PCSerial.h"
#include "modules.h"
#include "util.h"
#include "Dimensions.h"
<<<<<<< HEAD

#include <vector>
#include <sstream>
=======
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3

void PCSerial::rxTask()
{
    rxThread.signal_wait(1);    
    PCMessage* m;
    string mType;
<<<<<<< HEAD
    std::vector<string> mIns;
    string full;
=======
    string mIns[M];
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
    while(true)
    {             
        m = modules::pc->getNextMessage();
        mType = m->getMessageType();
<<<<<<< HEAD
        full = mType;
        mIns.clear();
        for (int i = 0; i < m->getLength(); i++)
        {
            mIns.push_back(m->getInstruction(i));   
            mType+= mIns[i];         
        }    
                   

       modules::sdCard->writeToLog("PC: "+full);
=======
        for (int i = 0; i < M; i++)
        {
            mIns[i] = m->getInstruction(i);
        }

        modules::sdCard->writeToLog("PC: "+mType+" "+mIns[0]+" "+mIns[1]+" "+mIns[2]);
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
         
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
<<<<<<< HEAD
                modules::pressureSensor->stopTimer();                             
                Dimensions d = {util::ToFloat(mIns[0]), util::ToFloat(mIns[1]), util::ToFloat(mIns[2]), util::ToFloat(mIns[3]), util::ToFloat(mIns[4])};                
                
=======
                modules::pressureSensor->stopTimer();
                Dimensions d = {util::ToFloat(mIns[0]), util::ToFloat(mIns[1]), util::ToFloat(mIns[2]), util::ToFloat(mIns[3]), util::ToFloat(mIns[4])};
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
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
<<<<<<< HEAD
            else if (mType == "setWirelessMode")
            {
                if (mIns[0] == "GSM")
                {
                    Wireless::setConnectionMode(Wireless::GSM);   
                }
                else if (mIns[0] == "XBEE")
                {
                    Wireless::setConnectionMode(Wireless::XBEE);
                }
                else if (mIns[0] == "None")
                {
                    Wireless::setConnectionMode(Wireless::NONE);
                }
                else
                {
                    util::printError("Please select GSM, XBEE or None");
                }
            }
=======
>>>>>>> fa0e297cc9ac7e3fc27fd0274cae3e2de0aea1e3
            else
            {
                util::printError("Unknown Command '"+mType+"'");
            }
        }
    }
}
