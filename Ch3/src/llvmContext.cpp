#include "llvmContext.h"

void LLVMContext::Initialize()
{

    // Open a new context and module.
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("Super Epic JIT", *context);

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<>>(*context);

}