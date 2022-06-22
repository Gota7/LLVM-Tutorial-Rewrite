#pragma once

#include "ast/exprAST.h"
#include "ast/functionAST.h"
#include "ast/numberExprAST.h"
#include "ast/prototypeAST.h"
#include "lexer.h"
#include "llvmContext.h"
#include <memory>
#include <vector>

// Parser for parsing the grammar into an AST.
struct Parser
{
    Lexer* lexer; // Lexer for reading the tokens.
    Lexeme currToken; // Current token.
    std::vector<std::unique_ptr<ExprAST>> parsedAST; // Parsed AST children. Not used in JIT mode.
    std::unique_ptr<LLVMContext> ctx = nullptr; // For JIT execution.

    // Parser constructor.
    Parser(Lexer* lexer) : lexer(lexer) {}

    // Parser in JIT mode.
    Parser(Lexer* lexer, std::unique_ptr<LLVMContext> ctx) : lexer(lexer), ctx(std::move(ctx)) {}

    // Fetch the next token.
    int NextToken();

    // Error function.
    static std::unique_ptr<ExprAST> LogError(const char* str);

    // Error function for prototype.
    static std::unique_ptr<PrototypeAST> LogErrorP(const char* str);

    // Parse an expression.
    std::unique_ptr<ExprAST> ParseExpression();

    // Parse a number expression.
    std::unique_ptr<NumberExprAST> ParseNumberExpression();

    // Parse an expression with parenthesis.
    std::unique_ptr<ExprAST> ParseParenExpression();

    // Parse an expression with an identifier in it.
    std::unique_ptr<ExprAST> ParseIdentifierExpression();

    // Parse an if expression.
    std::unique_ptr<ExprAST> ParseIfExpression();

    // Parse a for expression.
    std::unique_ptr<ExprAST> ParseForExpression();

    // Parse a var expression.
    std::unique_ptr<ExprAST> ParseVarExpression();

    // An identifier, number, or parenthesis expression.
    std::unique_ptr<ExprAST> ParsePrimaryExpression();

    // Parse a unary expression.
    std::unique_ptr<ExprAST> ParseUnaryExpression();

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