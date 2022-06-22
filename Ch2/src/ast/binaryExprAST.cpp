#include "binaryExprAST.h"

std::string BinaryExprAST::DebugString()
{
    return "(" + lhs->DebugString() + " " + op + " " + rhs->DebugString() + ")";
}