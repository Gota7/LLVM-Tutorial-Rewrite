#include "numberExprAST.h"

std::string NumberExprAST::DebugString()
{
    return std::to_string(value);
}

llvm::Value* NumberExprAST::Codegen(LLVMContext& ctx)
{
    return llvm::ConstantFP::get(*ctx.context, llvm::APFloat(value));
}