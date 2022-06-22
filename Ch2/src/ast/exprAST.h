#pragma once

#include <string>

// Base AST for expressions.
struct ExprAST
{

    // Get the debug string for how the tree looks.
    virtual std::string DebugString() { return ""; };

};