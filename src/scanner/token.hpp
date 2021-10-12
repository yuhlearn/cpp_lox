#ifndef _TOKEN_HPP
#define _TOKEN_HPP

#include <string>
#include <list>
#include <any>

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
        BOOLEAN,

        // Keywords.
        AND,
        CLASS,
        ELSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        VAR,
        WHILE,

        ENDOF
    };

    class Token
    {
    private:
        std::string typeToString() const;

    public:
        const TokenType type;
        const std::string lexeme;
        const std::any literal;
        const int line;

        Token(TokenType type, std::string lexeme, std::any literal, int line);
        std::string toString(void) const;
    };
}

#endif