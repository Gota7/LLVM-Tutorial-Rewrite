#include "functionAST.h"
#include "../lexer.h"
#include "llvm/IR/Verifier.h"

std::string FunctionAST::DebugString()
{
    return "def " + prototype->DebugString() + "\n" + body->DebugString();
}

llvm::Value* FunctionAST::Codegen(LLVMContext& ctx)
{

    // Transfer ownership of function but keep reference.
    auto& p = *prototype;
    ctx.functionProtos[prototype->name] = std::move(prototype);
    llvm::Function *theFunction = ctx.GetFunction(p.name);
    if (!theFunction) return nullptr;

    // If this is an operator, install it.
    if (p.IsBinaryOp()) Lexeme::operatorPrecedences[p.GetOpName()] = p.precedence;

    // Create a new basic block to start insertion into.
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*ctx.context, "entry", theFunction);
    ctx.builder->SetInsertPoint(bb);

    // Record the function arguments in the NamedValues map.
    ctx.namedValues.clear();
    for (auto &arg : theFunction->args()) ctx.namedValues[std::string(arg.getName())] = &arg;

    // Generate function.
    if (llvm::Value* retVal = body->Codegen(ctx))
    {

        // Finish off the function.
        ctx.builder->CreateRet(retVal);

        // Validate the generated code, checking for consistency, optimize it, then return it.
        llvm::verifyFunction(*theFunction);
        ctx.fpm->run(*theFunction);
        return theFunction;

    }

    // Error reading body, remove function.
    theFunction->eraseFromParent();
    return nullptr;

}