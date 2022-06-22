#pragma once

#include "exprAST.h"
#include <memory>
#include <string>
#include <vector>

// Var expression for mutable variables.
struct VarExprAST : ExprAST
{
    std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> varNames; // Variable names declared.
    std::unique_ptr<ExprAST> body; // Where variables can be used.

    // Create a new var expression.
    VarExprAST(std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> varNames, std::unique_ptr<ExprAST> body) : varNames(std::move(varNames)), body(std::move(body)) {}

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

}; 