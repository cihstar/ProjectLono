#include "Commands.h"

void buildCommandList()
{    
    PCCommand debug("debug", "Activate or deactivate debug mode. debug 0 = OFF, debug 1 = ON", debugCmd, 1);
    modules::pc->addCommand(debug);
    
    PCCommand getTime("getTime", "Read the current system time", getTimeCmd, 0);
    modules::pc->addCommand(getTime);
    
    PCCommand setTime("setTime", "Set the current system time manually. Date/Time format = dd/mm/yy hh:mm:ss", setTimeCmd, 2);
    modules::pc->addCommand(setTime);
    
    PCCommand directMode("directMode", "Enable direct communications with device modules. Options: GSM for GSM module. Type DirectMode to deactivate.", directModeCmd, 1);
    modules::pc->addCommand(directMode);
    
    PCCommand readPressure("readPressure", "Get current ADC reading from Presure Sensor.", readPressureCmd, 0);
    modules::pc->addCommand(readPressure);
    
    PCCommand setDimensions("setDimensions", "Set the physical rain gauge dimensions. Usage (all in m): setDimensions tubeRadius funnelRadius outTubeRadius outTubeWal pressureSensorTubeRadius startEmptyHeight endEmptyHeight", setDimensionsCmd, 7);
    modules::pc->addCommand(setDimensions);
    
    PCCommand setCalibration("setCalibration", "Set the calibration data for the sensor. Usage: setCalibration fullADC emptyADC fullHeight(m)", setCalibrationCmd, 3);
    modules::pc->addCommand(setCalibration);
    
    PCCommand setTiming("setTiming", "Set the data sampling timing paramaters. Usage: setTiming txInterval(ms) sampleInterval(ms) readsPerSample", setTimingCmd, 3);
    modules::pc->addCommand(setTiming);
    
    PCCommand setWirelessMode("setWirelessMode", "Set the Wireless TX mode of system. GSM, XBEE or NONE", setWirelessModeCmd, 1);
    modules::pc->addCommand(setWirelessMode);
    
}

void debugCmd(vector<string> &mIns)
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

void getTimeCmd(vector<string> &mIns)
{
    util::printInfo("System Time -> "+util::getTimeStamp());
}

void setTimeCmd(vector<string> &mIns)
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

void directModeCmd(vector<string> &mIns)
{
    if (mIns[0] == "GSM")
    {
        util::print("Now communicating direct to GSM Module. Type 'directMode' to deactivate");
        util::printBreak();
        modules::gsm->setPrint(true);
        modules::pc->setGsmMode(true);
    }
    else
    {
        util::printError("Please choose component for direct comms mode");
    }
}

void readPressureCmd(vector<string> &mIns)
{
    util::printInfo(util::ToString(modules::pressureSensor->read()));
}

void setDimensionsCmd(vector<string> &mIns)
{
    modules::pressureSensor->stopTimer();
    Dimensions d = {util::ToFloat(mIns[0]), util::ToFloat(mIns[1]), util::ToFloat(mIns[2]), util::ToFloat(mIns[3]), util::ToFloat(mIns[4]), util::ToFloat(mIns[5]), util::ToFloat(mIns[6])};
    modules::sdCard->writeDimensions(d);
    modules::pressureSensor->setDimensions(d);  
    modules::pressureSensor->start();    
}

void setCalibrationCmd(vector<string> &mIns)
{
    modules::pressureSensor->stopTimer();
    Calibrate c = {util::ToUint(mIns[0]), util::ToUint(mIns[1]), util::ToFloat(mIns[2])};
    modules::sdCard->writeCalibrateData(c);
    modules::pressureSensor->calibrate(c);
    modules::pressureSensor->start();   
}

void setTimingCmd(vector<string> &mIns)
{
    modules::pressureSensor->stopTimer();
    Timing t = {util::ToUintL(mIns[0]), util::ToUint(mIns[1]), util::ToUint(mIns[2])};
    modules::sdCard->writeTimingData(t);
    modules::pressureSensor->setTiming(t);
    modules::pressureSensor->start();
}

void setWirelessModeCmd(vector<string> &mIns)
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
