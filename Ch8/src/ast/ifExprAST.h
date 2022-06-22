#pragma once

#include "exprAST.h"
#include <memory>

// For if/then/else expressions.
struct IfExprAST : ExprAST
{
    std::unique_ptr<ExprAST> condAST; // Condition to be satisfied.
    std::unique_ptr<ExprAST> thenAST; // Code to run if condition is satisfied.
    std::unique_ptr<ExprAST> elseAST; // Code to run if condition is not satisfied.

    // Create a new if/then/else expression.
    IfExprAST(std::unique_ptr<ExprAST> condAST, std::unique_ptr<ExprAST> thenAST, std::unique_ptr<ExprAST> elseAST) : condAST(std::move(condAST)), thenAST(std::move(thenAST)), elseAST(std::move(elseAST)) {}

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

};