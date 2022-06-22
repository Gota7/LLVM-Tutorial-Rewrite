#pragma once

#include <fstream>
#include <map>
#include <string>

// Lexer token.
enum Token
{
    TOK_EOF = -1,
    TOK_DEF = -2,
    TOK_EXTERN = -3,
    TOK_IDENTIFIER = -4,
    TOK_NUMBER = -5,
    TOK_IF = -6,
    TOK_THEN = -7,
    TOK_ELSE = -8,
    TOK_FOR = -9,
    TOK_IN = -10,
    NUM_TOKS = 10
};

// For converting a token into a string.
static const char* TokenStrings[] = 
{
    "EndOfFile",
    "FunctionDef",
    "ExternFunction",
    "Identifier",
    "Number",
    "If",
    "Then",
    "Else",
    "For",
    "In"
};

// Precedence of each operator.
static const std::map<char, int> OperatorPrecedences =
{
    { '<', 10 },
    { '>', 10 },
    { '+', 20 },
    { '-', 20 },
    { '*', 40 },
    { '/', 40 },
    { '%', 40 }
};

// Standard lexeme.
struct Lexeme
{
    Token token; // Token for the lexeme.
    std::string identifier = ""; // TOK_IDENTIFIER.
    double number = 0; // TOK_NUMBER.

    // Empty constructor.
    Lexeme() {}

    // Default constructor.
    Lexeme(char& inputChar, std::istream* inStream);

    // Fetch the operator precedence.
    static int GetOperatorPrecedence(char token);

    // Debug output string.
    std::string DebugString();

};

// Lexer for parsing input.
struct Lexer
{
    std::istream* in; // Input stream.
    char lastChar = ' '; // Last character gotten.

    // Default constructor.
    Lexer(std::istream& in);

    // Fetch a lexeme from the input stream.
    Lexeme NextLexeme();

};