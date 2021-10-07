#include <parser/parser.hpp>
#include <repl/repl.hpp>

using namespace Lox;
using namespace std;

Parser::Parser(vector<Token> tokens) : tokens(tokens), current(0)
{
}

Parser::~Parser(void)
{
}

std::vector<std::shared_ptr<const Statement>> Parser::parse(void)
{
    auto statements = vector<shared_ptr<const Statement>>();

    while (!isAtEnd())
    {
        statements.push_back(declaration());
    }
    return statements;
}

Parser::ParseError Parser::error(Token token, string message)
{
    REPL::error(token, message);
    return ParseError();
}

void Parser::synchronize(void)
{
    advance();

    while (!isAtEnd())
    {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type)
        {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        default:
            advance();
        }
    }
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::ENDOF;
}

Token Parser::peek()
{
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}

Token Parser::advance(void)
{
    if (!isAtEnd())
        current++;

    return previous();
}

Token Parser::consume(TokenType type, string message)
{
    if (check(type))
        return advance();

    throw error(peek(), message);
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;

    return peek().type == type;
}

template <typename... Args>
bool Parser::match(Args... types)
{
    for (TokenType type : {types...})
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

shared_ptr<Expression> Parser::expression(void)
{
    return assignment();
}

shared_ptr<Statement> Parser::declaration(void)
{
    try
    {
        if (match(TokenType::FUN))
            return function("function");
        if (match(TokenType::VAR))
            return varDeclaration();
        return statement();
    }
    catch (ParseError &error)
    {
        synchronize();
        return nullptr;
    }
}

shared_ptr<Statement> Parser::statement(void)
{
    if (match(TokenType::FOR))
        return forStatement();

    if (match(TokenType::IF))
        return ifStatement();

    if (match(TokenType::PRINT))
        return printStatement();

    if (match(TokenType::RETURN))
        return returnStatement();

    if (match(TokenType::WHILE))
        return whileStatement();

    if (match(TokenType::LEFT_BRACE))
    {
        auto blockStmt = block();
        return make_shared<Block>(blockStmt);
    }

    return expressionStatement();
}

shared_ptr<Statement> Parser::forStatement(void)
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    shared_ptr<Statement> initializer;

    if (match(TokenType::SEMICOLON))
        initializer = nullptr;
    else if (match(TokenType::VAR))
        initializer = varDeclaration();
    else
        initializer = expressionStatement();

    shared_ptr<Expression> condition = nullptr;

    if (!check(TokenType::SEMICOLON))
        condition = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    shared_ptr<Expression> increment = nullptr;

    if (!check(TokenType::RIGHT_PAREN))
        increment = expression();

    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    shared_ptr<Statement> body = statement();

    if (increment != nullptr)
    {
        auto stmtList = make_shared<list<shared_ptr<Statement>>>();
        stmtList->push_back(body);
        stmtList->push_back(make_shared<ExpressionStatement>(increment));

        body = make_shared<Block>(stmtList);
    }

    if (condition == nullptr)
        condition = make_shared<Literal>(
            make_shared<TokenType>(TokenType::BOOLEAN),
            make_shared<boost::any>(true));

    body = make_shared<While>(condition, body);

    if (initializer != nullptr)
    {
        auto stmtList = make_shared<list<shared_ptr<Statement>>>();
        stmtList->push_back(initializer);
        stmtList->push_back(body);

        body = make_shared<Block>(stmtList);
    }

    return body;
}

shared_ptr<Statement> Parser::ifStatement(void)
{
    consume(TokenType::LEFT_PAREN, " Expect '(' after 'if'.");
    shared_ptr<Expression> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    shared_ptr<Statement> thenBranch = statement();
    shared_ptr<Statement> elseBranch = nullptr;

    if (match(TokenType::ELSE))
        elseBranch = statement();

    return make_shared<If>(condition, thenBranch, elseBranch);
}

shared_ptr<Statement> Parser::printStatement(void)
{
    shared_ptr<Expression> value = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return make_shared<Print>(value);
}

shared_ptr<Statement> Parser::returnStatement(void)
{
    shared_ptr<Token> keyword = make_shared<Token>(previous());
    shared_ptr<Expression> value = nullptr;

    if (!check(TokenType::SEMICOLON))
    {
        value = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after return value.");

    return make_shared<Return>(keyword, value);
}

std::shared_ptr<Statement> Parser::varDeclaration(void)
{
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    shared_ptr<Expression> initializer = nullptr;

    if (match(TokenType::EQUAL))
        initializer = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

    return make_shared<Var>(make_shared<Token>(name), initializer);
}

std::shared_ptr<Statement> Parser::whileStatement(void)
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    shared_ptr<Expression> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    shared_ptr<Statement> body = statement();

    return make_shared<While>(condition, body);
}

shared_ptr<Statement> Parser::expressionStatement(void)
{
    shared_ptr<Expression> value = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after expression.");

    return make_shared<ExpressionStatement>(value);
}

std::shared_ptr<Statement> Parser::function(std::string kind)
{
    shared_ptr<Token> name = make_shared<Token>(
        consume(TokenType::IDENTIFIER, "Expect " + kind + " name."));
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    auto parameters = make_shared<list<shared_ptr<Token>>>();

    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (parameters->size() >= 255)
            {
                error(peek(), "Can't have more than 255 parameters.");
            }
            parameters->push_back(make_shared<Token>(
                consume(TokenType::IDENTIFIER, "Expect parameter name.")));
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    shared_ptr<list<shared_ptr<Statement>>> body = block();

    return make_shared<Function>(name, parameters, body);
}

shared_ptr<list<shared_ptr<Statement>>> Parser::block(void)
{
    auto statements = make_shared<list<shared_ptr<Statement>>>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        statements->push_back(declaration());

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

shared_ptr<Expression> Parser::assignment(void)
{
    shared_ptr<Expression> expr = orOp();

    if (match(TokenType::EQUAL))
    {
        Token equals = previous();
        shared_ptr<Expression> value = assignment();

        if (shared_ptr<Variable> variable = dynamic_pointer_cast<Variable>(expr))
            return make_shared<Assign>(variable->name, value);

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

shared_ptr<Expression> Parser::orOp(void)
{
    shared_ptr<Expression> expr = andOp();

    while (match(TokenType::OR))
    {
        shared_ptr<Token> op = make_shared<Token>(previous());
        shared_ptr<Expression> right = andOp();
        expr = make_shared<Logical>(expr, op, right);
    }
    return expr;
}

shared_ptr<Expression> Parser::andOp(void)
{
    shared_ptr<Expression> expr = equality();

    while (match(TokenType::AND))
    {
        shared_ptr<Token> op = make_shared<Token>(previous());
        shared_ptr<Expression> right = equality();
        expr = make_shared<Logical>(expr, op, right);
    }
    return expr;
}

shared_ptr<Expression> Parser::equality(void)
{
    shared_ptr<Expression> expr = comparison();

    while (match(TokenType::BANG_EQUAL,
                 TokenType::EQUAL_EQUAL))
    {
        shared_ptr<Token> op = make_shared<Token>(previous());
        shared_ptr<Expression> right = comparison();
        expr = make_shared<Binary>(expr, op, right);
    }
    return expr;
}

shared_ptr<Expression> Parser::comparison(void)
{
    shared_ptr<Expression> expr = term();

    while (match(TokenType::GREATER,
                 TokenType::GREATER_EQUAL,
                 TokenType::LESS,
                 TokenType::LESS_EQUAL))
    {
        shared_ptr<Token> op = make_shared<Token>(previous());
        shared_ptr<Expression> right = term();
        expr = make_shared<Binary>(expr, op, right);
    }
    return expr;
}

shared_ptr<Expression> Parser::term(void)
{
    shared_ptr<Expression> expr = factor();

    while (match(TokenType::MINUS,
                 TokenType::PLUS))
    {
        shared_ptr<Token> op = make_shared<Token>(previous());
        shared_ptr<Expression> right = factor();
        expr = make_shared<Binary>(expr, op, right);
    }
    return expr;
}

shared_ptr<Expression> Parser::factor(void)
{
    shared_ptr<Expression> expr = unary();

    while (match(TokenType::SLASH,
                 TokenType::STAR))
    {
        shared_ptr<Token> op = make_shared<Token>(previous());
        shared_ptr<Expression> right = unary();
        expr = make_shared<Binary>(expr, op, right);
    }
    return expr;
}

shared_ptr<Expression> Parser::unary(void)
{
    if (match(TokenType::BANG,
              TokenType::MINUS))
    {
        shared_ptr<Token> op = make_shared<Token>(previous());
        shared_ptr<Expression> right = unary();
        return make_shared<Unary>(op, right);
    }
    return call();
}

shared_ptr<Expression> Parser::finishCall(std::shared_ptr<Expression> callee)
{
    auto arguments = make_shared<list<shared_ptr<Expression>>>();

    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (arguments->size() >= 255)
                error(peek(), "Can't have more than 255 arguments.");
            arguments->push_back(expression());
        } while (match(TokenType::COMMA));
    }

    auto paren = make_shared<Token>(
        consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments."));
    return make_shared<Call>(callee, paren, arguments);
}

shared_ptr<Expression> Parser::call(void)
{
    shared_ptr<Expression> expr = primary();

    while (true)
    {
        if (match(TokenType::LEFT_PAREN))
            expr = finishCall(expr);
        else
            break;
    }

    return expr;
}

shared_ptr<Expression> Parser::primary(void)
{
    if (match(TokenType::BOOLEAN))
        return make_shared<Literal>(make_shared<TokenType>(TokenType::BOOLEAN),
                                    make_shared<boost::any>(previous().literal));
    if (match(TokenType::NIL))
        return make_shared<Literal>(make_shared<TokenType>(TokenType::NIL),
                                    make_shared<boost::any>(previous().literal));
    if (match(TokenType::NUMBER))
        return make_shared<Literal>(make_shared<TokenType>(TokenType::NUMBER),
                                    make_shared<boost::any>(previous().literal));
    if (match(TokenType::STRING))
        return make_shared<Literal>(make_shared<TokenType>(TokenType::STRING),
                                    make_shared<boost::any>(previous().literal));
    if (match(TokenType::IDENTIFIER))
        return make_shared<Variable>(make_shared<Token>(previous()));

    if (match(TokenType::LEFT_PAREN))
    {
        shared_ptr<Expression> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expected expression.");
}
