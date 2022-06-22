#include "lexer.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "parser.h"
#include <iostream>
#include <memory>
#include <sstream>

// Test bench for parser.
int main()
{

    // Read test file.
    std::fstream testStream("test.k", std::ios_base::openmode::_S_in);

    // Create context.
    std::unique_ptr<LLVMContext> ctx = std::make_unique<LLVMContext>();
    ctx->InitJIT();
    ctx->Initialize();
    
    // Do the parsing.
    Lexer lex(testStream);
    Parser parser(&lex, std::move(ctx));
    parser.ParseInputMain();
    testStream.close();

}