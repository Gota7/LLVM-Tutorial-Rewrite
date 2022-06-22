#include "prototypeAST.h"

bool PrototypeAST::IsUnaryOp() const
{
    return isOperator && args.size() == 1;
}

bool PrototypeAST::IsBinaryOp() const
{
    return isOperator && args.size() == 2;
}

char PrototypeAST::GetOpName()
{
    assert(IsUnaryOp() || IsBinaryOp());
    return name[name.size() - 1]; // Get last character.
}

std::string PrototypeAST::DebugString()
{
    std::string ret = "proto " + name + "(";
    for (int i = 0; i < args.size(); i++)
    {
        ret += args[i];
        if (i != args.size() - 1) ret += " ";
    }
    return ret + ")";
}

llvm::Value* PrototypeAST::Codegen(LLVMContext& ctx)
{

    // Make the function type: double(double, double, ...).
    std::vector<llvm::Type*> doubles(args.size(), llvm::Type::getDoubleTy(*ctx.context));

    // Get function type and declare function.
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*ctx.context), doubles, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, ctx.module.get());

    // Set names for all arguments and return.
    unsigned idx = 0;
    for (auto &arg : f->args()) arg.setName(args[idx++]);
    return f;

}