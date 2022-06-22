#include "functionAST.h"

std::string FunctionAST::DebugString()
{
    return "def " + prototype->DebugString() + "\n" + body->DebugString();
}