#include "callExprAST.h"

std::string CallExprAST::DebugString()
{
    std::string argParams = "(";
    for (int i = 0; i < args.size(); i++)
    {
        argParams += args[i]->DebugString();
        if (i != args.size() - 1) argParams += ", ";
    }
    return callee + argParams + ")";
}