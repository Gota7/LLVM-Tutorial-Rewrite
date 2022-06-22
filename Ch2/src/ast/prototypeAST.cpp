#include "prototypeAST.h"

std::string PrototypeAST::DebugString()
{
    std::string ret = "proto " + name + "(";
    for (int i = 0; i < args.size(); i++)
    {
        ret += args[i];
        if (i != args.size() - 1) ret += " ";
    }
    return ret + ")";
}