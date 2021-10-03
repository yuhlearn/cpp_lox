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
    if (match(TokenType::PRINT))
        return printStatement();

    if (match(TokenType::LEFT_BRACE))
    {
        auto blockStmt = block();
        return make_shared<Block>(blockStmt);
    }

    return expressionStatement();
}

shared_ptr<Statement> Parser::printStatement(void)
{
    shared_ptr<Expression> value = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return make_shared<Print>(value);
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

shared_ptr<Statement> Parser::expressionStatement(void)
{
    shared_ptr<Expression> value = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after expression.");

    return make_shared<ExpressionStatement>(value);
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
    shared_ptr<Expression> expr = equality();

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
    return primary();
}

shared_ptr<Expression> Parser::primary(void)
{
    if (match(TokenType::BOOLEAN))
        return make_shared<Literal>(make_shared<TokenType>(TokenType::BOOLEAN),
                                    make_shared<boost::any>(previous().literal));
    if (match(TokenType::NIL))
        return make_shared<Literal>(make_shared<TokenType>(TokenType::NIL),
                                    nullptr);
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
