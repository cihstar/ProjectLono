#ifndef __UTIL_H__
#define __UTIL_H__

#include <sstream>
#include <string>
#include "mbed.h"

namespace util
{
    void printInfo(std::string s);
    void printDebug(std::string s);
    void printError(std::string s);
    void print(std::string s);
    void printBreak();
    int ToInt(std::string s);
    uint16_t ToUint(std::string s);
    float ToFloat(std::string s);

    template <typename T>
    std::string ToString(T val)
    {
        std::stringstream stream;
        stream << val;
        return stream.str();
    }
   
    bool isNumber(char c);
    static bool timeSet = false;
    bool setTime(string date, string stime);
    string getTimeStamp();
}

#endif