#include "functionAST.h"
#include "llvm/IR/Verifier.h"

std::string FunctionAST::DebugString()
{
    return "def " + prototype->DebugString() + "\n" + body->DebugString();
}

llvm::Value* FunctionAST::Codegen(LLVMContext& ctx)
{

    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function* theFunction = ctx.module->getFunction(prototype->name);
    if (!theFunction) theFunction = (llvm::Function*)prototype->Codegen(ctx);
    if (!theFunction) return nullptr;

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

        // Validate the generated code, checking for consistency.
        llvm::verifyFunction(*theFunction);
        return theFunction;

    }

    // Error reading body, remove function.
    theFunction->eraseFromParent();
    return nullptr;

}