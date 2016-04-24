/* Defins the PCCommand Object 
    name: string defining the command to type to call the function
    description: string explainig the command. 'help' or '?' will display this 
    function: function pointer to a function with 1 argument , vector<string> &mIns, containing the arguments
    numArgs: int the number of arguments this function takes.
    */
    
#ifndef __PC_CMD_H__
#define __PC_CMD_H__

#include "mbed.h"
#include <string>
#include <vector>

class PCCommand
{
    public:
        PCCommand();
        PCCommand(string _name, string _description, void(*_function)(vector<string> &mIns), int numArgs);
        string getName();
        string getDescription();
        int getArgumentNumber();
        void execute(vector<string> &mIns);
    
    private:
        string name;
        int argumentNumber;
        string description;
        void (*function)(vector<string> &mIns);    
};

#endif