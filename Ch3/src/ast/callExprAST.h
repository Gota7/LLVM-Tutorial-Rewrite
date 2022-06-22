#pragma once

#include "exprAST.h"
#include <memory>
#include <string>
#include <vector>

// Expression for function calls.
struct CallExprAST : ExprAST
{
    std::string callee; // What is being called.
    std::vector<std::unique_ptr<ExprAST>> args; // Function arguments.

    // Create a new call expression.
    CallExprAST(std::string& callee, std::vector<std::unique_ptr<ExprAST>> args) : callee(callee), args(std::move(args)) {}

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

};