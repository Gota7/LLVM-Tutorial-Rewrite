#include "exprAST.h"
#include "../parser.h"

llvm::Value* ExprAST::LogErrorV(const char* err)
{
    Parser::LogError(err);
    return nullptr;
}