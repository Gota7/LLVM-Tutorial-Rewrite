#pragma once

#include "exprAST.h"
#include <string>

// Variable expressions for representing variables.
struct VariableExprAST : ExprAST
{
    std::string name; // Name of the variable.

    // Create a new variable expression.
    VariableExprAST(std::string& name) : name(name) {}

    // V-Funtions.
    virtual std::string DebugString();

};