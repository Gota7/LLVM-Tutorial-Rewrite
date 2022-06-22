#pragma once

#include "exprAST.h"
#include "prototypeAST.h"
#include <string>
#include <memory>

// Actual function definition.
struct FunctionAST : ExprAST
{
    std::unique_ptr<PrototypeAST> prototype; // Function prototype.
    std::unique_ptr<ExprAST> body; // Function body.

    // Create a new function declaration.
    FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprAST> body) : prototype(std::move(prototype)), body(std::move(body)) {}

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

};