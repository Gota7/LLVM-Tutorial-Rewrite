#include "ifExprAST.h"

std::string IfExprAST::DebugString()
{
    return "if " + condAST->DebugString() + " then\n" + thenAST->DebugString() + "\nelse\n" + elseAST->DebugString();
}

llvm::Value* IfExprAST::Codegen(LLVMContext& ctx)
{

    // Generate condition.
    llvm::Value* condV = condAST->Codegen(ctx);
    if (!condV) return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.0.
    condV = ctx.builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*ctx.context, llvm::APFloat(0.0)), "ifcond");
    llvm::Function* theFunction = ctx.builder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*ctx.context, "then", theFunction);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*ctx.context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*ctx.context, "ifcont");
    ctx.builder->CreateCondBr(condV, thenBB, elseBB);

    // Emit then value.
    ctx.builder->SetInsertPoint(thenBB);
    llvm::Value* thenV = thenAST->Codegen(ctx);
    if (!thenV) return nullptr;
    ctx.builder->CreateBr(mergeBB);

    // Codegen of 'then' can change the current block, update thenBB for the PHI.
    thenBB = ctx.builder->GetInsertBlock();

    // Emit else block.
    theFunction->getBasicBlockList().push_back(elseBB);
    ctx.builder->SetInsertPoint(elseBB);
    llvm::Value* elseV = elseAST->Codegen(ctx);
    if (!elseV) return nullptr;
    ctx.builder->CreateBr(mergeBB);

    // Codegen of 'else' can change the current block, update elseBB for the PHI.
    elseBB = ctx.builder->GetInsertBlock();

    // Emit merge block.
    theFunction->getBasicBlockList().push_back(mergeBB);
    ctx.builder->SetInsertPoint(mergeBB);
    llvm::PHINode* pn = ctx.builder->CreatePHI(llvm::Type::getDoubleTy(*ctx.context), 2, "iftmp");

    // Finish.
    pn->addIncoming(thenV, thenBB);
    pn->addIncoming(elseV, elseBB);
    return pn;

}