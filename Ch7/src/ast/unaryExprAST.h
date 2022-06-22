#pragma once

#include "exprAST.h"
#include <memory>

// Unary expression.
struct UnaryExprAST : ExprAST
{
    char opcode; // Character that activates the operator.
    std::unique_ptr<ExprAST> operand; // Expression to operate on.

    // Create a new unary operator expression.
    UnaryExprAST(char opcode, std::unique_ptr<ExprAST> operand) : opcode(opcode), operand(std::move(operand)) {}

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

};