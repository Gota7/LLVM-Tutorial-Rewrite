#include "binaryExprAST.h"
#include "variableExprAST.h"

std::string BinaryExprAST::DebugString()
{
    return "(" + lhs->DebugString() + " " + op + " " + rhs->DebugString() + ")";
}

llvm::Value* BinaryExprAST::Codegen(LLVMContext& ctx)
{

    // Special case '=' because we don't want to emit the LHS as an expression.
    if (op == '=') {

        // Assignment requires the LHS to be an identifier.
        VariableExprAST* lhse = static_cast<VariableExprAST*>(lhs.get());
        if (!lhse) return LogErrorV("Destination of '=' must be a variable.");

        // Codegen the RHS.
        llvm::Value* val = rhs->Codegen(ctx);
        if (!val) return nullptr;

        // Look up the name.
        llvm::Value* variable = ctx.namedValues[lhse->name];
        if (!variable) return LogErrorV("Unknown variable name.");

        // Store item.
        ctx.builder->CreateStore(val, variable);
        return val;

    }

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
            break;
    }

    // Attempt to find user defined operator.
    llvm::Function* f = ctx.GetFunction(std::string("binary") + op);
    assert(f && "Binary operator not found!");
    llvm::Value* ops[2] = { lhs, rhs };
    return ctx.builder->CreateCall(f, ops, "binop");

}