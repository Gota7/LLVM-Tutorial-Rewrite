#include "variableExprAST.h"

std::string VariableExprAST::DebugString()
{
    return name;
}

llvm::Value* VariableExprAST::Codegen(LLVMContext& ctx)
{
    llvm::Value* ret = ctx.namedValues[name];
    if (!ret) return LogErrorV(("Unknown variable name " + name + ".").c_str());
    else return ctx.builder->CreateLoad(llvm::Type::getDoubleTy(*ctx.context), ret, name.c_str());
}