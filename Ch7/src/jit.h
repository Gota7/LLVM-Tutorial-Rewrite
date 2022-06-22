#pragma once

#include "kaleidoscopeJIT.h"
#include <memory>

// JIT for execution.
struct JIT
{
    std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit; // Kaleidoscope JIT that is pulled out of nowhere because LLVM tutorial.
    llvm::ExitOnError exitOnErr; // For exiting on errors.
    
    // Initialize the JIT.
    void Initialize();

};