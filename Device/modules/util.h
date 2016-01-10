#ifndef __UTIL_H__
#define __UTIL_H__

#include <sstream>
#include <string>

template <typename T>
string ToString(T val)
{
    stringstream stream;
    stream << val;
    return stream.str();
}

#endif