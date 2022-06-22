#include "forExprAST.h"

std::string ForExprAST::DebugString()
{
    return "for " + varName + " = " + start->DebugString() + ", " + cond ->DebugString() + ", " + step->DebugString() + "\n" + body->DebugString();
}

llvm::Value* ForExprAST::Codegen(LLVMContext& ctx)
{

    // Emit the start code first, without 'variable' in scope.
    llvm::Value* startVal = start->Codegen(ctx);
    if (!startVal) return nullptr;

    // Make the new basic block for the loop header, inserting after current block.
    llvm::Function* theFunction = ctx.builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* preheaderBB = ctx.builder->GetInsertBlock();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*ctx.context, "loop", theFunction);

    // Insert an explicit fall through from the current block to the loopBB.
    ctx.builder->CreateBr(loopBB);

    // Start insertion in loopBB.
    ctx.builder->SetInsertPoint(loopBB);

    // Start the PHI node with an entry for start.
    llvm::PHINode* variable = ctx.builder->CreatePHI(llvm::Type::getDoubleTy(*ctx.context), 2, varName);
    variable->addIncoming(startVal, preheaderBB);

    // Within the loop, the variable is defined equal to the PHI node. If it shadows an existing variable, we have to restore it, so save it now.
    llvm::Value* oldVal = ctx.namedValues[varName];
    ctx.namedValues[varName] = variable;

    /*  Emit the body of the loop.  This, like any other expr, can change the
        current BB.  Note that we ignore the value computed by the body, but don't
        allow an error. */
    if (!body->Codegen(ctx)) return nullptr;

    // Emit the step value.
    llvm::Value *stepVal = nullptr;
    if (step)
    {
        stepVal = step->Codegen(ctx);
        if (!stepVal) return nullptr;
    }
    else
    {
        stepVal = llvm::ConstantFP::get(*ctx.context, llvm::APFloat(1.0)); // If not specified, use 1.0.
    }
    llvm::Value* nextVar = ctx.builder->CreateFAdd(variable, stepVal, "nextvar");

    // Compute the end condition.
    llvm::Value* endCond = cond->Codegen(ctx);
    if (!endCond) return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.0.
    endCond = ctx.builder->CreateFCmpONE(endCond, llvm::ConstantFP::get(*ctx.context, llvm::APFloat(0.0)), "loopcond");

    // Create the "after loop" block and insert it.
    llvm::BasicBlock* loopEndBB = ctx.builder->GetInsertBlock();
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*ctx.context, "afterloop", theFunction);

    // Insert the conditional branch into the end of LoopEndBB.
    ctx.builder->CreateCondBr(endCond, loopBB, afterBB);

    // Any new code will be inserted in AfterBB.
    ctx.builder->SetInsertPoint(afterBB);

    // Add a new entry to the PHI node for the backedge.
    variable->addIncoming(nextVar, loopEndBB);

    // Restore the unshadowed variable.
    if (oldVal) ctx.namedValues[varName] = oldVal;
    else ctx.namedValues.erase(varName);

    // For expr always returns 0.0.
    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*ctx.context));

}