#pragma once

#include "exprAST.h"
#include <memory>

// Expression for binary operators.
struct BinaryExprAST : ExprAST
{
    char op; // Operator to use.
    std::unique_ptr<ExprAST> lhs; // Left hand statement.
    std::unique_ptr<ExprAST> rhs; // Right hand statement.

    // Create a new variable expression.
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    // V-Funtions.
    virtual std::string DebugString();

};