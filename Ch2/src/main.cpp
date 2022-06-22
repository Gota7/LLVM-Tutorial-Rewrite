#include "lexer.h"
#include "parser.h"
#include <iostream>
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
    printf("$\n");
    for (auto& x : parser.parsedAST)
    {
        printf("%s\n", x->DebugString().c_str());
    }
    testStream.close();

}