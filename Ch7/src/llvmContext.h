#pragma once

#include "jit.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>
#include <memory>
#include <string>

struct PrototypeAST;

// For making passing around LLVM information a lot easier.
struct LLVMContext
{
    std::unique_ptr<llvm::LLVMContext> context; // Context of LLVM data.
    std::unique_ptr<llvm::Module> module; // Contains all the functions.
    std::unique_ptr<llvm::IRBuilder<>> builder; // For building instructions.
    std::map<std::string, llvm::AllocaInst*> namedValues; // Variable definitions.
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm; // For optimizizing code.
    std::unique_ptr<JIT> jit = nullptr; // For real time execution of code.
    std::map<std::string, std::unique_ptr<PrototypeAST>> functionProtos; // For storing declared function prototypes.

    // Init JIT. Do this first.
    void InitJIT();

    // Initialize the context.
    void Initialize();

    // Find a function.
    llvm::Function* GetFunction(std::string name);

    // Create alloca instruction in entry block of function.
    llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* theFunction, const std::string& varName);
    
};