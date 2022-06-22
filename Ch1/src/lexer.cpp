#include "lexer.h"

Lexeme::Lexeme(char& inputChar, std::istream* inStream)
{

    // EOF check.
    if (inStream->eof())
    {
        token = TOK_EOF;
    }

    // Is identifier. _[a-zA-Z]?[a-zA-Z0-9]*
    else if (isalpha(inputChar) || inputChar == '_')
    {
        while (isalnum(inputChar))
        {
            identifier += inputChar;
            inStream->get(inputChar);
        }

        if (identifier == "def")
        {
            token = TOK_DEF;
        }
        else if (identifier == "extern")
        {
            token = TOK_EXTERN;
        }
        else
        {
            token = TOK_IDENTIFIER;
        }
    }

    // Is a number, read it.
    else if (isdigit(inputChar) || inputChar == '.')
    {
        std::string numStr = "";
        do
        {
            numStr += inputChar;
            inStream->get(inputChar);
        } while (isdigit(inputChar) || inputChar == '.');
        token = TOK_NUMBER;
        number = strtod(numStr.c_str(), nullptr);
    }

    // Who knows what it is, just use ASCII.
    else
    {
        token = (Token)inputChar;
        inStream->get(inputChar);
    }

}

Lexer::Lexer(std::istream& in)
{
    this->in = &in;
}

Lexeme Lexer::NextLexeme()
{
    
    // Skip whitespace.
    while (isspace(lastChar))
    {
        in->get(lastChar);
    }

    // Skip comments.
    if (lastChar == '#')
    {
        do
        {
            in->get(lastChar);
        } while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');
    }

    // Finally return the lexeme.
    return Lexeme(lastChar, in);

}

std::string Lexeme::DebugString()
{
    if (token < NUM_TOKS)
    {
        if (token == TOK_IDENTIFIER) return identifier;
        else if (token == TOK_NUMBER) return std::to_string(number);
        else return TokenStrings[token];
    }
    else
    {
        return std::string(1, token);
    }
}