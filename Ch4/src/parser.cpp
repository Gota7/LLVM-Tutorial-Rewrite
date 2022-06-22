#include "parser.h"
#include "ast/binaryExprAST.h"
#include "ast/callExprAST.h"
#include "ast/variableExprAST.h"

int Parser::NextToken()
{
    currToken = lexer->NextLexeme();
    return currToken.token;
}

std::unique_ptr<ExprAST> Parser::LogError(const char* str)
{
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::LogErrorP(const char* str)
{
    LogError(str);
    return nullptr;
}

// primary parseBinOps
std::unique_ptr<ExprAST> Parser::ParseExpression()
{
    auto lhs = ParsePrimaryExpression();
    if (!lhs) return nullptr;
    return ParseRHSBinOps(0, std::move(lhs));
}

// number
std::unique_ptr<NumberExprAST> Parser::ParseNumberExpression()
{
    auto result = std::make_unique<NumberExprAST>(currToken.number);
    NextToken(); // Consume number.
    return std::move(result);
}

// ( expression )
std::unique_ptr<ExprAST> Parser::ParseParenExpression()
{
    NextToken(); // Eat (.
    auto v = ParseExpression();
    if (!v) return nullptr;
    else if (currToken.token != ')') return LogError("Expected ')'.");
    NextToken(); // Eat ).
    return v;
}

// identifier
// identifier ( (expression (, expression)*)? )
std::unique_ptr<ExprAST> Parser::ParseIdentifierExpression()
{
    std::string idName = currToken.identifier;
    NextToken(); // Eat the identifier.

    // Simple variable reference.
    if (currToken.token != '(') return std::make_unique<VariableExprAST>(idName);

    // Call.
    NextToken(); // Eat (.
    std::vector<std::unique_ptr<ExprAST>> args;
    if (currToken.token != ')')
    {
        while (true)
        {
            if (auto arg = ParseExpression()) args.push_back(std::move(arg)); // Parse expression.
            else return nullptr;

            if (currToken.token == ')') break; // End of function.

            if (currToken.token != ',') return LogError("Expected ')' or ',' in argument list.");
            NextToken(); // Eat comma to go to next argument.
        }
    }
    NextToken(); // Eat ).
    return std::make_unique<CallExprAST>(idName, std::move(args));

}

// identifierExpression
// numberExpression
// parenthesisExpression
std::unique_ptr<ExprAST> Parser::ParsePrimaryExpression()
{
    switch (currToken.token)
    {
        case TOK_IDENTIFIER:
            return ParseIdentifierExpression();
        case TOK_NUMBER:
            return ParseNumberExpression();
        case '(':
            return ParseParenExpression();
        default:
            fprintf(stderr, ("Unexpected " + currToken.DebugString() + "\n").c_str());
            return LogError("Unknown token when expecting an expression!");
    }
}

// (OP primaryExpression)*
std::unique_ptr<ExprAST> Parser::ParseRHSBinOps(int currPrecedence, std::unique_ptr<ExprAST> lhs)
{

    // Keep going until no more operators.
    while (true)
    {
        int tokenPrecedence = Lexeme::GetOperatorPrecedence(currToken.token);
        if (tokenPrecedence < currPrecedence) return lhs; // New operator doesn't have higher precedence.

        // Eat the current operator.
        char op = currToken.token;
        NextToken();

        // Parse primaryExpression.
        auto rhs = ParsePrimaryExpression();
        if (!rhs) return nullptr;

        // Compare next precedence.
        int nextPrecedence = Lexeme::GetOperatorPrecedence(currToken.token);
        if (tokenPrecedence < nextPrecedence)
        {
            rhs = ParseRHSBinOps(tokenPrecedence + 1, std::move(rhs));
            if (!rhs) return nullptr;
        }

        // Merge left and right sides.
        lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));

    }

}

// id ( id* )
std::unique_ptr<PrototypeAST> Parser::ParsePrototype()
{

    // Invalid function name.
    if (currToken.token != TOK_IDENTIFIER) return LogErrorP("Expected function name in prototype.");

    // Skip identifier.
    std::string fnName = currToken.identifier;
    NextToken();

    // Find (.
    if (currToken.token != '(') return LogErrorP("Expected '(' in prototype.");

    // Argument parsing.
    std::vector<std::string> argNames;
    while (NextToken() == TOK_IDENTIFIER) argNames.push_back(currToken.identifier);
    if (currToken.token != ')') return LogErrorP("Expected ')' in prototype.");

    // Success.
    NextToken(); // Eat ).
    return std::make_unique<PrototypeAST>(fnName, std::move(argNames));

}

// "def" prototype expression
std::unique_ptr<FunctionAST> Parser::ParseDefinition()
{
    NextToken(); // Eat def.

    // Parse prototype.
    auto proto = ParsePrototype();
    if (!proto) return nullptr;

    // Parse expression.
    if (auto e = ParseExpression()) return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
    return nullptr;

}

// expression
std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpression()
{

    // We can't execute non-functions so make each top level a function.
    if (auto e = ParseExpression())
    {
        auto proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
    }
    return nullptr;

}

// "extern" prototype
std::unique_ptr<PrototypeAST> Parser::ParseExtern()
{
    NextToken(); // Eat extern.
    return ParsePrototype();
}

void Parser::HandleDefinition()
{
    std::unique_ptr<ExprAST> expr = ParseDefinition();
    if (expr)
    {

        // JIT mode.
        if (ctx)
        {
            if (auto* fnIR = expr->Codegen(*ctx))
            {
                fprintf(stderr, "Read function definition:");
                fnIR->print(llvm::errs());
                fprintf(stderr, "\n");
                ctx->jit->exitOnErr(ctx->jit->jit->addModule(llvm::orc::ThreadSafeModule(std::move(ctx->module), std::move(ctx->context))));
                ctx->Initialize();
            }
        }
        
        // Normal mode.
        else
        {
            fprintf(stderr, "Parsed a function definition.\n");
            parsedAST.push_back(std::move(expr));
        }

    }
    else NextToken();
}

void Parser::HandleExtern()
{
    std::unique_ptr<PrototypeAST> expr = ParseExtern();
    if (expr)
    {

        // JIT mode.
        if (ctx)
        {
            if (auto* fnIR = expr->Codegen(*ctx))
            {
                fprintf(stderr, "Read extern: ");
                fnIR->print(llvm::errs());
                fprintf(stderr, "\n");
                ctx->functionProtos[expr->name] = std::move(expr);
            }
        }

        // Normal mode.
        else
        {
            fprintf(stderr, "Parsed an extern.\n");
            parsedAST.push_back(std::move(expr));
        }

    }
    else NextToken();
}

void Parser::HandleTopLevelExpression()
{
    std::unique_ptr<ExprAST> expr = ParseTopLevelExpression();
    if (expr)
    {

        // JIT mode.
        if (ctx)
        {
            if (expr->Codegen(*ctx))
            {

                // Resource track the anonymous expression.
                auto rt = ctx->jit->jit->getMainJITDylib().createResourceTracker();

                // Make module thread safe.
                auto tsm = llvm::orc::ThreadSafeModule(std::move(ctx->module), std::move(ctx->context));
                ctx->jit->exitOnErr(ctx->jit->jit->addModule(std::move(tsm), rt));
                ctx->Initialize();

                // Search the JIT for the __anon_expr symbol.
                auto exprSymbol = ctx->jit->exitOnErr(ctx->jit->jit->lookup("__anon_expr"));

                // This is a jank as hell way to call a function and LLVM should be ashamed.
                double (*fp)() = (double (*)())(intptr_t)exprSymbol.getAddress();
                fprintf(stderr, "Evaluated to %f.\n", fp());

                // Delete the anonymous expression module from the JIT.
                ctx->jit->exitOnErr(rt->remove());

            }
        }

        // Normal mode.
        else
        {
            fprintf(stderr, "Parsed a top-level expression.\n");
            parsedAST.push_back(std::move(expr));
        }
        
    }
    else NextToken();
}

void Parser::ParseInputMain()
{
    NextToken();
    while (true)
    {
        fprintf(stderr, "Ready> ");
        switch (currToken.token)
        {
            case TOK_EOF:
                return;
            case ';': // Ignore top-level semicolons.
                NextToken();
                break;
            case TOK_DEF:
                HandleDefinition();
                break;
            case TOK_EXTERN:
                HandleExtern();
                break;
            default:
                HandleTopLevelExpression();
                break;
        }
    }
}