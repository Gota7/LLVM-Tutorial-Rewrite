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
    
    // Do the parsing.
    Lexer lex(testStream);
    Parser parser(&lex);
    parser.ParseInputMain();
    
    // Output results.
    LLVMContext ctx;
    ctx.Initialize();
    for (auto& x : parser.parsedAST)
    {
        x->Codegen(ctx);
    }
    std::error_code err;
    llvm::raw_fd_ostream outBc("test.bc", err);
    llvm::WriteBitcodeToFile(*ctx.module, outBc);
    outBc.close();
    testStream.close();

}