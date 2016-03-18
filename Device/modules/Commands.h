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
void readAdcCmd(vector<string> &mIns);
void adcCmd(vector<string> &mIns);
void setDimensionsCmd(vector<string> &mIns);
void setCalibrationCmd(vector<string> &mIns);
void setTimingCmd(vector<string> &mIns);
void setWirelessModeCmd(vector<string> &mIns);
void getHeightCmd(vector<string> &mIns);
void setFunnelCmd(vector<string> &mIns);
void stopCmd(vector<string> &mIns);
void startCmd(vector<string> &mIns);

#endif
