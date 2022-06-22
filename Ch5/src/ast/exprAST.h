#pragma once

#include "../llvmContext.h"
#include "llvm/IR/Value.h"
#include <memory>
#include <string>

// Base AST for expressions.
struct ExprAST
{

    // Log a value error.
    static llvm::Value* LogErrorV(const char* err);

    // Get the debug string for how the tree looks.
    virtual std::string DebugString() { return ""; };

    // Get an LLVM value from compiling the code.
    virtual llvm::Value* Codegen(LLVMContext& ctx) { return nullptr; }

};