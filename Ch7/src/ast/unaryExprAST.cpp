#include "unaryExprAST.h"

std::string UnaryExprAST::DebugString()
{
    return opcode + operand->DebugString();
}

llvm::Value* UnaryExprAST::Codegen(LLVMContext& ctx)
{

    // Get operand value.
    llvm::Value* operandV = operand->Codegen(ctx);
    if (!operandV) return nullptr;

    // Find operator function.
    llvm::Function* f = ctx.GetFunction(std::string("unary") + opcode);
    if (!f) return LogErrorV("Unknown unary operator.");

    // Return final call.
    return ctx.builder->CreateCall(f, operandV, "unop");

}