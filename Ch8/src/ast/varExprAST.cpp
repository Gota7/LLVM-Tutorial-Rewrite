#include "varExprAST.h"

std::string VarExprAST::DebugString()
{
    std::string ret = "var ";
    for (auto& x : varNames)
    {
        ret += x.first + " = " + x.second->DebugString();
    }
    return ret + " in\n" + body->DebugString();
}

llvm::Value* VarExprAST::Codegen(LLVMContext& ctx)
{

    // Setup vars.
    std::vector<llvm::AllocaInst*> oldBindings;
    llvm::Function* theFunction = ctx.builder->GetInsertBlock()->getParent();

    // Register all variables and emit their initializer.
    for (unsigned i = 0, e = varNames.size(); i != e; ++i)
    {
        const std::string& varName = varNames[i].first;
        ExprAST* init = varNames[i].second.get();

        // Emit the initializer before adding the variable to scope, this prevents
        // the initializer from referencing the variable itself, and permits stuff
        // like this:
        //  var a = 1 in
        //    var a = a in ...   # refers to outer 'a'.
        llvm::Value* initVal;
        if (init)
        {
            initVal = init->Codegen(ctx);
            if (!initVal) return nullptr;
        }
        else
        { 
            initVal = llvm::ConstantFP::get(*ctx.context, llvm::APFloat(0.0)); // If not specified, use 0.0.
        }

        // Create alloca.
        llvm::AllocaInst* allocaD = ctx.CreateEntryBlockAlloca(theFunction, varName);
        ctx.builder->CreateStore(initVal, allocaD);

        // Remember the old variable binding so that we can restore the binding when we unrecurse.
        oldBindings.push_back(ctx.namedValues[varName]);

        // Remember this binding.
        ctx.namedValues[varName] = allocaD;

    }

    // Codegen the body, now that all vars are in scope.
    llvm::Value* bodyVal = body->Codegen(ctx);
    if (!bodyVal) return nullptr;

    // Pop all our variables from scope.
    for (unsigned i = 0, e = varNames.size(); i != e; ++i) ctx.namedValues[varNames[i].first] = oldBindings[i];

    // Return the body computation.
    return bodyVal;

}