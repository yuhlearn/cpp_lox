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

shared_ptr<Expression> Parser::parse(void)
{
    try
    {
        return expression();
    }
    catch (ParseError &error)
    {
        return NULL;
    }
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
    return equality();
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
        return make_shared<Literal>(TokenType::BOOLEAN, previous().literal);

    if (match(TokenType::NIL))
        return make_shared<Literal>(TokenType::NIL, nullptr);

    if (match(TokenType::NUMBER))
        return make_shared<Literal>(TokenType::NUMBER, previous().literal);

    if (match(TokenType::STRING))
        return make_shared<Literal>(TokenType::STRING, previous().literal);

    if (match(TokenType::LEFT_PAREN))
    {
        shared_ptr<Expression> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Exprected ')' after expression.");
        return make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expected expression.");
}
