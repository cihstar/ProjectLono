/* Defins the PCCommand Object 
    name: string defining the command to type to call the function
    description: string explainig the command. 'help' or '?' will display this 
    function: function pointer to a function with 1 argument , vector<string> &mIns, containing the arguments
    numArgs: int the number of arguments this function takes.
    */

#include "PCCommand.h"

PCCommand::PCCommand(string _name, string _description, void(*_function)(vector<string> &mIns), int numArgs) :
name(_name), description(_description), function(_function), argumentNumber(numArgs)
{
}

PCCommand::PCCommand() : name("null"), description("null"), function(NULL)
{}

string PCCommand::getName()
{
    return name;
}

string PCCommand::getDescription()
{
    return description;
}

void PCCommand::execute(vector<string> &mIns)
{
    function(mIns);
}

int PCCommand::getArgumentNumber()
{
    return argumentNumber;
}