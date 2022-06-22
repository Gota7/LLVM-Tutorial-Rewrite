#pragma once

#include "exprAST.h"

// Number expression is used for representing numeric values (what else would it do?).
struct NumberExprAST : ExprAST
{
    double value; // Number expression is just a number value.

    // Create a new number expression.
    NumberExprAST(double value) : value(value) {}

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

};