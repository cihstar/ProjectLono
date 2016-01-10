#ifndef __UTIL_H__
#define __UTIL_H__

#include <sstream>
#include <string>

namespace util
{

    void printInfo(std::string s);
    void printDebug(std::string s);
    void printError(std::string s);
    void printBreak();
    int ToInt(std::string s);

    template <typename T>
    std::string ToString(T val)
    {
        std::stringstream stream;
        stream << val;
        return stream.str();
    }
}

#endif