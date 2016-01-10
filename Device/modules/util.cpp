#include "util.h"
#include "modules.h"
#include <sstream>
#include <string>

namespace util
{
    int ToInt(std::string s)
    {
        return atoi(s.c_str());
    }

    void printInfo(string s)
    {   
        modules::pc->print("Info: "+s);
    }

    void printDebug(string s)
    {
        #ifdef DEBUG
            if (modules::pc->getDebug())
                modules::pc->print("Debug: "+s);
        #endif
    }

    void printError(string s)
    {
        modules::pc->print("Error: "+s);
    }

    void printBreak()
    {
        modules::pc->print("\r\n---------------------------------------\r\n");
    }
}
