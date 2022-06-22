#include "parser.h"
#include "ast/binaryExprAST.h"
#include "ast/callExprAST.h"
#include "ast/forExprAST.h"
#include "ast/ifExprAST.h"
#include "ast/unaryExprAST.h"
#include "ast/varExprAST.h"
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
    auto lhs = ParseUnaryExpression();
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

// "if" expression "then" expression "else" expression
std::unique_ptr<ExprAST> Parser::ParseIfExpression()
{

    // Eat if.
    NextToken();

    // Condition.
    auto condExpr = ParseExpression();
    if (!condExpr) return nullptr;

    // Then token.
    if (currToken.token != TOK_THEN) return LogError("Expected then.");
    NextToken(); // Eat then.

    // Then expression.
    auto thenExpr = ParseExpression();
    if (!thenExpr) return nullptr;

    // Else token.
    if (currToken.token != TOK_ELSE) return LogError("Expected else.");
    NextToken(); // Eat else.

    // Else expression.
    auto elseExpr = ParseExpression();
    if (!elseExpr) return nullptr;

    // Return the final AST.
    return std::make_unique<IfExprAST>(std::move(condExpr), std::move(thenExpr), std::move(elseExpr));

}

// "for" identifier = expression, expression (, expression)? "in" expression
std::unique_ptr<ExprAST> Parser::ParseForExpression()
{

    // Eat for.
    NextToken();

    // Identifier.
    if (currToken.token != TOK_IDENTIFIER) return LogError("Expected identifier after for.");
    std::string idName = currToken.identifier;
    NextToken(); // Eat identifier.

    // Equals sign.
    if (currToken.token != '=') return LogError("Expected '=' after for.");
    NextToken(); // Eat '='.

    // Start expression.
    auto start = ParseExpression();
    if (!start) return nullptr;
    if (currToken.token != ',') return LogError("Expected ',' after for start value.");
    NextToken();

    // Condition expression.
    auto cond = ParseExpression();
    if (!cond) return nullptr;

    // The step value is optional.
    std::unique_ptr<ExprAST> step;
    if (currToken.token == ',')
    {
        NextToken();
        step = ParseExpression();
        if (!step) return nullptr;
    }

    // In token.
    if (currToken.token != TOK_IN) return LogError("Expected 'in' after for.");
    NextToken(); // Eat 'in'.

    // Finally, the body.
    auto body = ParseExpression();
    if (!body) return nullptr;

    // Finished parsing.
    return std::make_unique<ForExprAST>(idName, std::move(start), std::move(cond), std::move(step), std::move(body));

}

// "var" ID (= expression)? (, ID (= expression)?)* "in" expression
// 
std::unique_ptr<ExprAST> Parser::ParseVarExpression()
{

    // Setup, eat var.
    NextToken();
    std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> varNames;

    // At least one variable name is required.
    if (currToken.token != TOK_IDENTIFIER) return LogError("Expected identifier after var.");

    // Run to get vars.
    while (true)
    {
        std::string name = currToken.identifier;
        NextToken(); // Eat identifier.

        // Read the optional initializer.
        std::unique_ptr<ExprAST> init = nullptr;
        if (currToken.token == '=')
        {
            NextToken(); // Eat the '='.
            init = ParseExpression();
            if (!init) return nullptr;
        }
        varNames.push_back(std::make_pair(name, std::move(init)));

        // End of var list, exit loop.
        if (currToken.token != ',') break;
        NextToken(); // Eat the ','.
        if (currToken.token != TOK_IDENTIFIER)
        return LogError("Expected identifier list after var.");

    }

    // At this point, we have to have 'in'.
    if (currToken.token != TOK_IN) return LogError("Expected 'in' keyword after 'var'.");
    NextToken(); // Eat in.

    // Parse body.
    auto body = ParseExpression();
    if (!body) return nullptr;

    // Finally done.
    return std::make_unique<VarExprAST>(std::move(varNames), std::move(body));

}

// op expression
std::unique_ptr<ExprAST> Parser::ParseUnaryExpression()
{
    // If the current token is not an operator, it must be a primary expr.
    if (!isascii(currToken.token) || currToken.token == '(' || currToken.token == ',') return ParsePrimaryExpression();

    // If this is a unary operator, read it.
    int opc = currToken.token;
    NextToken();
    if (auto operand = ParseUnaryExpression()) return std::make_unique<UnaryExprAST>(opc, std::move(operand));
    return nullptr;

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
        case TOK_IF:
            return ParseIfExpression();
        case TOK_FOR:
            return ParseForExpression();
        case TOK_VAR:
            return ParseVarExpression();
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

        // Parse.
        auto rhs = ParseUnaryExpression();
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
// binary letter number?  ( id* )
std::unique_ptr<PrototypeAST> Parser::ParsePrototype()
{

    // Get prototype type.
    std::string fnName;
    OperatorKind kind = OP_IDENTIFIER;
    unsigned precedence = 30;
    switch (currToken.token) {
        case TOK_IDENTIFIER:
            fnName = currToken.identifier;
            kind = OP_IDENTIFIER;
            NextToken();
            break;
        case TOK_UNARY:
            NextToken();
            if (!isascii(currToken.token))
            return LogErrorP("Expected unary operator.");
            fnName = "unary";
            fnName += (char)currToken.token;
            kind = OP_UNARY;
            NextToken();
            break;
        case TOK_BINARY:
            NextToken();
            if (!isascii(currToken.token)) return LogErrorP("Expected binary operator.");
            fnName = "binary";
            fnName += (char)currToken.token;
            kind = OP_BINARY;
            NextToken();

            // Read the precedence if present.
            if (currToken.token == TOK_NUMBER)
            {
                if (currToken.number < 1 || currToken.number > 100) return LogErrorP("Invalid precedence: must be 1..100.");
                precedence = (unsigned)currToken.number;
                NextToken();
            }
            break;
        default:
            return LogErrorP("Expected function name in prototype.");
    }

    // Find (.
    if (currToken.token != '(') return LogErrorP("Expected '(' in prototype.");

    // Argument parsing.
    std::vector<std::string> argNames;
    while (NextToken() == TOK_IDENTIFIER) argNames.push_back(currToken.identifier);
    if (currToken.token != ')') return LogErrorP("Expected ')' in prototype.");

    // Verify right number of names for operator.
    if (kind && argNames.size() != kind) return LogErrorP("Invalid number of operands for operator.");

    // Success.
    NextToken(); // Eat ).
    return std::make_unique<PrototypeAST>(fnName, std::move(argNames), kind, precedence);

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