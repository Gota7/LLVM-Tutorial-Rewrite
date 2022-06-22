#include "binaryExprAST.h"

std::string BinaryExprAST::DebugString()
{
    return "(" + lhs->DebugString() + " " + op + " " + rhs->DebugString() + ")";
}

llvm::Value* BinaryExprAST::Codegen(LLVMContext& ctx)
{

    // Grab left and right hand sides.
    llvm::Value* lhs = this->lhs->Codegen(ctx);
    llvm::Value* rhs = this->rhs->Codegen(ctx);
    if (!lhs || !rhs) return nullptr;

    // Apply operator.
    switch (op)
    {
        case '<':
            return ctx.builder->CreateUIToFP(
                ctx.builder->CreateFCmpULT(lhs, rhs, "cmptmp"),
                llvm::Type::getDoubleTy(*ctx.context),
                "booltmp"
            );
        case '>':
            return ctx.builder->CreateUIToFP(
                ctx.builder->CreateFCmpUGT(lhs, rhs, "cmptmp"),
                llvm::Type::getDoubleTy(*ctx.context),
                "booltmp"
            );
        case '+':
            return ctx.builder->CreateFAdd(lhs, rhs, "addtmp");
        case '-':
            return ctx.builder->CreateFSub(lhs, rhs, "subtmp");
        case '*':
            return ctx.builder->CreateFMul(lhs, rhs, "multmp");
        case '/':
            return ctx.builder->CreateFDiv(lhs, rhs, "divtmp");
        case '%':
            return ctx.builder->CreateFRem(lhs, rhs, "modtmp");
        default:
            return LogErrorV("Invalid binary operator.");
    }

}