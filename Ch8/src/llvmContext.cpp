#include "llvmContext.h"
#include "ast/prototypeAST.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetIntrinsicInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include <sstream>

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

    // Promote allocas to registers.
    fpm->add(llvm::createPromoteMemoryToRegisterPass());

    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm->add(llvm::createInstructionCombiningPass());

    // Reassociate expressions.
    fpm->add(llvm::createReassociatePass());

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

llvm::AllocaInst* LLVMContext::CreateEntryBlockAlloca(llvm::Function* theFunction, const std::string& varName)
{
    llvm::IRBuilder<> tmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
    return tmpB.CreateAlloca(llvm::Type::getDoubleTy(*context), 0, varName.c_str());
}

void LLVMContext::OutputToObject(llvm::raw_pwrite_stream& dest)
{

    // Initialize the target registry etc.
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    // Get target.
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    // Print an error and exit if we couldn't find the requested target.
    if (!target)
    {
        llvm::errs() << error;
        return;
    }

    // Init data.
    auto cpu = "generic";
    auto features = "";
    llvm::TargetOptions opt;
    auto rm = llvm::Optional<llvm::Reloc::Model>();
    auto theTargetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);
    module->setDataLayout(theTargetMachine->createDataLayout());

    // Make sure to output object file.
    llvm::legacy::PassManager pass;
    if (theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile))
    {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }
    pass.run(*module);
    dest.flush();

}