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

    // Parse input.
    Lexer lex(testStream);
    Parser parser(&lex);
    parser.ParseInputMain();
    
    // Compile code.
    LLVMContext ctx;
    ctx.Initialize();
    for (auto& x : parser.parsedAST)
    {
        x->Codegen(ctx);
    }
    testStream.close();

    // Output object file.
    auto filename = "test.o";
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    if (ec)
    {
        llvm::errs() << "Could not open file: " << ec.message();
        return 1;
    }
    ctx.OutputToObject(dest);
    llvm::outs() << "Wrote " << filename << ".\n";
    
    // Done.
    return 0;

}