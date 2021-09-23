#ifndef _SCANNER_HPP
#define _SCANNER_HPP

#include <scanner/token.hpp>
#include <string>
#include <list>
#include <memory>
#include <unordered_map>
#include <boost/any.hpp>

namespace Lox
{
    class Scanner
    {
    private:
        const std::unordered_map<std::string, TokenType> keywords =
            {{"and", TokenType::AND},
             {"class", TokenType::CLASS},
             {"else", TokenType::ELSE},
             {"false", TokenType::FALSE},
             {"for", TokenType::FOR},
             {"fun", TokenType::FUN},
             {"if", TokenType::IF},
             {"nil", TokenType::NIL},
             {"or", TokenType::OR},
             {"print", TokenType::PRINT},
             {"return", TokenType::RETURN},
             {"super", TokenType::SUPER},
             {"this", TokenType::THIS},
             {"true", TokenType::TRUE},
             {"var", TokenType::VAR},
             {"while", TokenType::WHILE}};
        const std::string source;
        std::list<Token> tokens;
        int line;
        int start;
        int current;
        bool isAtEnd(void);
        bool isAlpha(char c);
        bool isAlphaNumeric(char c);
        char advance(void);
        void addToken(TokenType tokenType, boost::any literal);
        bool match(char expected);
        char peek();
        char peekNext();
        void matchString(void);
        void matchNumber(void);
        void matchIdentifier(void);
        void scanToken(void);

    public:
        Scanner(std::string source);
        const std::list<Token> &scanTokens(void);
    };
}

#endif