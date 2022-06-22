#pragma once

#include "exprAST.h"
#include <string>
#include <vector>

// Expression for function declarations (not to be confused with definitions).
struct PrototypeAST : ExprAST
{
    std::string name; // Name of the function.
    std::vector<std::string> args; // Function arguments.

    // Create a new function declaration.
    PrototypeAST(const std::string& name, std::vector<std::string> args) : name(name), args(args) {}

    // V-Funtions.
    virtual std::string DebugString();

};