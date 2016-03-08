#include "PCCommand.h"

PCCommand::PCCommand(string _name, string _description, void(*_function)(vector<string> &mIns), int numArgs) :
name(_name), description(_description), function(_function), argumentNumber(numArgs)
{
}

PCCommand::PCCommand() : name("GH"), description("SS"), function(NULL)
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