#pragma once

#include "exprAST.h"
#include <memory>
#include <string>

// For loop expressions.
struct ForExprAST : ExprAST
{
    std::string varName; // Name of the variable to use in for loop.
    std::unique_ptr<ExprAST> start; // Statement at the start of the for loop.
    std::unique_ptr<ExprAST> cond; // Condition statement for the for loop.
    std::unique_ptr<ExprAST> step; // Modifier at the end of the for loop.
    std::unique_ptr<ExprAST> body; // Code to execute inside of the for loop.

    // Constructor.
    ForExprAST(std::string varName, std::unique_ptr<ExprAST> start, std::unique_ptr<ExprAST> cond, std::unique_ptr<ExprAST> step, std::unique_ptr<ExprAST> body) : varName(varName), start(std::move(start)), cond(std::move(cond)), step(std::move(step)), body(std::move(body)) {}

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

};