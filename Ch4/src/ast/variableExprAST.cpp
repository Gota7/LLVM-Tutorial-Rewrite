#include "variableExprAST.h"

std::string VariableExprAST::DebugString()
{
    return name;
}

llvm::Value* VariableExprAST::Codegen(LLVMContext& ctx)
{
    llvm::Value* ret = ctx.namedValues[name];
    if (!ret) return LogErrorV("Unknown variable name.");
    else return ret;
}