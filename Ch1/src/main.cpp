#include "lexer.h"
#include <iostream>
#include <sstream>

// Test bench for lexer.
int main()
{

    // Read test file.
    std::fstream testStream("test.k", std::ios_base::openmode::_S_in);
    
    // Read through the stream and display the lexemes.
    Lexer lex(testStream);
    Lexeme lexeme;
    do
    {
        lexeme = lex.NextLexeme();
        std::cout << lexeme.DebugString() << "\n";
    } while (lexeme.token != TOK_EOF);
    testStream.close();
    return 0;

}