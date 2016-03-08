#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "modules.h"
#include "util.h"
#include "Dimensions.h"
#include "PCCommand.h"

#include <vector>
#include <sstream>

void buildCommandList();
void debugCmd(vector<string> &mIns);
void getTimeCmd(vector<string> &mIns);
void setTimeCmd(vector<string> &mIns);
void directModeCmd(vector<string> &mIns);
void readPressureCmd(vector<string> &mIns);
void setDimensionsCmd(vector<string> &mIns);
void setCalibrationCmd(vector<string> &mIns);
void setTimingCmd(vector<string> &mIns);
void setWirelessModeCmd(vector<string> &mIns);

#endif