#pragma once

#include "ast/exprAST.h"
#include "ast/functionAST.h"
#include "ast/numberExprAST.h"
#include "ast/prototypeAST.h"
#include "lexer.h"
#include <memory>
#include <vector>

// Parser for parsing the grammar into an AST.
struct Parser
{
    Lexer* lexer; // Lexer for reading the tokens.
    Lexeme currToken; // Current token.
    std::vector<std::unique_ptr<ExprAST>> parsedAST; // Parsed AST children.

    // Parser constructor.
    Parser(Lexer* lexer) : lexer(lexer) {}

    // Fetch the next token.
    int NextToken();

    // Error function.
    std::unique_ptr<ExprAST> LogError(const char* str);

    // Error function for prototype.
    std::unique_ptr<PrototypeAST> LogErrorP(const char* str);

    // Parse an expression.
    std::unique_ptr<ExprAST> ParseExpression();

    // Parse a number expression.
    std::unique_ptr<NumberExprAST> ParseNumberExpression();

    // Parse an expression with parenthesis.
    std::unique_ptr<ExprAST> ParseParenExpression();

    // Parse an expression with an identifier in it.
    std::unique_ptr<ExprAST> ParseIdentifierExpression();

    // An identifier, number, or parenthesis expression.
    std::unique_ptr<ExprAST> ParsePrimaryExpression();

    // Parse right hand binary operators.
    std::unique_ptr<ExprAST> ParseRHSBinOps(int currPrecedence, std::unique_ptr<ExprAST> lhs);

    // Parse a function declaration.
    std::unique_ptr<PrototypeAST> ParsePrototype();

    // Parse a function definition.
    std::unique_ptr<FunctionAST> ParseDefinition();

    // Parse a top level expression.
    std::unique_ptr<FunctionAST> ParseTopLevelExpression();

    // Parse an external function declaration.
    std::unique_ptr<PrototypeAST> ParseExtern();

    // Handle a function definition.
    void HandleDefinition();

    // Handle an external function declaration.
    void HandleExtern();

    // Handle a top level expression.
    void HandleTopLevelExpression();

    // Main parsing function.
    void ParseInputMain();

};