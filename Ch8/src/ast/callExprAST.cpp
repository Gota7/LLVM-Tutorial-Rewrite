#include "callExprAST.h"

std::string CallExprAST::DebugString()
{
    std::string argParams = "(";
    for (int i = 0; i < args.size(); i++)
    {
        argParams += args[i]->DebugString();
        if (i != args.size() - 1) argParams += ", ";
    }
    return callee + argParams + ")";
}

llvm::Value* CallExprAST::Codegen(LLVMContext& ctx)
{

    // Look up the name in the global module table.
    llvm::Function* calleeF = ctx.GetFunction(callee);
    if (!calleeF) return LogErrorV(("Unknown function " + callee + " referenced.").c_str());

    // If argument mismatch error.
    if (calleeF->arg_size() != args.size()) return LogErrorV("Incorrect # arguments passed.");

    // Compile call.
    std::vector<llvm::Value*> argsV;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        argsV.push_back(args[i]->Codegen(ctx));
        if (!argsV.back())
        return nullptr;
    }
    return ctx.builder->CreateCall(calleeF, argsV, "calltmp");
    
}