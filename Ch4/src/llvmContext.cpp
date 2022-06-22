#include "llvmContext.h"
#include "ast/prototypeAST.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

void LLVMContext::InitJIT()
{
    jit = std::make_unique<JIT>();
    jit->Initialize();
}

void LLVMContext::Initialize()
{

    // Open a new context and module.
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("Super Epic JIT", *context);
    if (jit) module->setDataLayout(jit->jit->getDataLayout());

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<>>(*context);

    // Add optimization passes.
    fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module.get());

    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm->add(llvm::createInstructionCombiningPass());

    // Reassociate expressions.
    fpm->add(llvm::createReassociatePass());
    
    // Eliminate Common SubExpressions.
    fpm->add(llvm::createGVNPass());

    // Simplify the control flow graph (deleting unreachable blocks, etc).
    fpm->add(llvm::createCFGSimplificationPass());

    // Finally initialize.
    fpm->doInitialization();

}

llvm::Function* LLVMContext::GetFunction(std::string name)
{
    
    // First, see if the function has already been added to the current module.
    if (auto *f = module->getFunction(name)) return f;

    // If not, check whether we can codegen the declaration from some existing prototype.
    auto fi = functionProtos.find(name);
    if (fi != functionProtos.end()) return (llvm::Function*)fi->second->Codegen(*this);

    // If no existing prototype exists, return null.
    return nullptr;

}