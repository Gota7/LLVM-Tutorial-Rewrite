#pragma once

#include "../llvmContext.h"
#include "exprAST.h"
#include "llvm/IR/Value.h"
#include <string>
#include <vector>

// Operator kind.
enum OperatorKind
{
    OP_IDENTIFIER,
    OP_UNARY,
    OP_BINARY
};

// Expression for function declarations (not to be confused with definitions).
struct PrototypeAST : ExprAST
{
    std::string name; // Name of the function.
    std::vector<std::string> args; // Function arguments.
    bool isOperator; // If this AST represents a custom operator.
    unsigned precedence; // Operator precedence if a custom operator.

    // Create a new function declaration.
    PrototypeAST(const std::string& name, std::vector<std::string> args, bool isOperator = false, unsigned precedence = 0) : name(name), args(args), isOperator(isOperator), precedence(precedence) {}

    // If it is a unary operator.
    bool IsUnaryOp() const;

    // If it is a binary operator.
    bool IsBinaryOp() const;

    // Get the operator name.
    char GetOpName();

    // V-Funtions.
    virtual std::string DebugString();
    virtual llvm::Value* Codegen(LLVMContext& ctx);

};