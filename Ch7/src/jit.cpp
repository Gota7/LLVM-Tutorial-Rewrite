#include "jit.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

void JIT::Initialize()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    jit = exitOnErr(llvm::orc::KaleidoscopeJIT::Create());
}