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
    
    uint16_t ToUint(std::string s)
    {
        return atoi(s.c_str());
    }
    
    float ToFloat(std::string s)
    {
        return atof(s.c_str());
    }

    void printInfo(string s)
    {           
        string str = "Info: "+s;
        modules::sdCard->writeToLog(str);        
        modules::pc->print(str);
    }

    void printDebug(string s)
    {        
        #ifdef DEBUG
            if (modules::pc->getDebug())
            {                
                string str = "Debug: "+s;
                modules::sdCard->writeToLog(str);
                modules::pc->print(str);
            }
        #endif
    }

    void printError(string s)
    {        
        string str = "Error: "+s;
        modules::sdCard->writeToLog(str);
        modules::pc->print(str);
    }

    void printBreak()
    {
        modules::pc->print("\r\n---------------------------------------\r\n");
    }
    
    void print(string s)
    {
        modules::sdCard->writeToLog(s);        
        modules::pc->print(s);
    }
    
    bool isNumber(char c)
    {
        return (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9');
    }
    
    bool setTime(string date, string stime)
    {
        if (date.length() != 10 && stime.length() != 8)
        {
            return false;
        }
        if (date != "" && stime !=""
            && isNumber(date[0]) && isNumber(date[1])
            && date[2] == '/'
            && isNumber(date[3]) && isNumber(date[4])
            && date[5] == '/'
            && isNumber(date[6]) && isNumber(date[7]) && isNumber(date[8]) && isNumber(date[9])
            && isNumber(stime[0]) && isNumber(stime[1])
            && stime[2] == ':'
            && isNumber(stime[3]) && isNumber(stime[4])
            && stime[5] == ':'
            && isNumber(stime[6]) && isNumber(stime[7]))
        {
            time_t raw;
            struct tm* t;
            int day = util::ToInt(date.substr(0,2));
            int month = util::ToInt(date.substr(3,2));
            int year = util::ToInt(date.substr(6,4));
            int hour = util::ToInt(stime.substr(0,2));
            int min = util::ToInt(stime.substr(3,2));
            int sec = util::ToInt(stime.substr(6,2));
            if  ((month >=1 && month <=12) &&
                (year >= 1900 && year <= 2100) &&
                (hour >= 0 && hour <=23) &&
                (min >= 0 && min <=59) &&
                (sec >= 0 && sec <=59))
            {    
                switch (month)
                {
                    case 2:
                        //Feburary, leap year?
                        if (year % 4 == 0)
                        {
                            if (!(day >=0 && day <= 29))
                            {
                                return false;
                            }
                        }
                        else
                        {
                            if (!(day >=0 && day <= 28))
                            {
                                return false;
                            }  
                        }
                        break;
                    //30 days:                    
                    case 4:
                    case 6:
                    case 9:
                    case 11:
                        if (!(day >=0 && day <=30))
                        {
                            return false;
                        }   
                        break;
                   //31 days
                   case 1:
                   case 3:
                   case 5:
                   case 7:
                   case 8:
                   case 10:
                   case 12:
                        if (!(day >=0 && day <=31))
                        {
                            return false;
                        }   
                        break;                      
                }            
                time(&raw);
                t = localtime(&raw);                
                t->tm_mday = day;                
                t->tm_mon = month - 1; 
                t->tm_year = year - 1900; 
                t->tm_hour = hour;
                t->tm_min = min;                 
                t->tm_sec = sec;                 
                raw = mktime(t);                
                set_time((unsigned int long)raw);            
                timeSet = true; 
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    string getTimeStamp()
    {
        if(timeSet)
        {            
            time_t seconds = time(NULL);
            struct tm* t = localtime(&seconds);
            
            int i[6];
            i[0] = t->tm_mday;
            i[1] = t->tm_mon+1;
            i[2] = t->tm_year+1900;
            i[3] = t->tm_hour;
            i[4] = t->tm_min;
            i[5] = t->tm_sec;

            std::stringstream sk;
            if (i[0] < 10)
            {
                sk << "0";
            }
            sk << i[0];
            sk << "/";
            if (i[1] < 10)
            {
                sk << "0";
            }
            sk << i[1];
            sk << "/" ;
            sk << i[2];
            sk << " ";
            if (i[3] < 10)
            {
                sk << "0";
            }
            sk << i[3];
            sk << ":";
            if (i[4] < 10)
            {
                sk << "0";
            }
            sk << i[4];
            sk << ":";
            if (i[5] < 10)
            {
                sk << "0";
            }
            sk << i[5];
            return sk.str();
        }
        else
        {
            return "NO_TIME_SET";
        }
    }
}
