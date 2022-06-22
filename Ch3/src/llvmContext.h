#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>
#include <memory>
#include <string>

// For making passing around LLVM information a lot easier.
struct LLVMContext
{
    std::unique_ptr<llvm::LLVMContext> context; // Context of LLVM data.
    std::unique_ptr<llvm::Module> module; // Contains all the functions.
    std::unique_ptr<llvm::IRBuilder<>> builder; // For building instructions.
    std::map<std::string, llvm::Value*> namedValues; // Variable definitions.

    // Initialize the context.
    void Initialize();
    
};