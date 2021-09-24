#ifndef _TOKEN_HPP
#define _TOKEN_HPP

#include <string>
#include <list>
#include <boost/any.hpp>

namespace Lox
{
    enum class TokenType
    {
        // Single-character tokens.
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,

        ENDOF
    };

    class Token
    {
    private:
        const TokenType type;
        const std::string lexeme;
        const boost::any literal;
        const int line;

        std::string typeToString() const;

    public:
        Token(TokenType type, std::string lexeme, boost::any literal, int line);
        std::string toString(void) const;
    };
}

#endif