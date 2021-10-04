#ifndef _PARSER_HPP
#define _PARSER_CPP

#include <scanner/token.hpp>
#include <ast/expression.hpp>
#include <ast/statement.hpp>
#include <vector>
#include <exception>
#include <memory>

namespace Lox
{
    class Parser
    {
    private:
        struct ParseError : public std::exception
        {
            const char *what() const throw()
            {
                return "ParseError";
            }
        };

        const std::vector<Token> tokens;
        int current;

        ParseError error(Token token, std::string message);
        void synchronize(void);
        bool isAtEnd(void);
        Token peek(void);
        Token previous(void);
        Token advance(void);
        Token consume(TokenType type, std::string message);
        bool check(TokenType type);
        template <typename... Args>
        bool match(Args... types);

        std::shared_ptr<Statement> printStatement(void);
        std::shared_ptr<Statement> varDeclaration(void);
        std::shared_ptr<Statement> whileStatement(void);
        std::shared_ptr<Statement> expressionStatement(void);
        std::shared_ptr<std::list<std::shared_ptr<Statement>>> block(void);
        std::shared_ptr<Expression> assignment(void);
        std::shared_ptr<Expression> orOp(void);
        std::shared_ptr<Expression> andOp(void);
        std::shared_ptr<Expression> expression(void);
        std::shared_ptr<Statement> statement(void);
        std::shared_ptr<Statement> ifStatement(void);
        std::shared_ptr<Statement> forStatement(void);
        std::shared_ptr<Statement> declaration(void);
        std::shared_ptr<Expression> equality(void);
        std::shared_ptr<Expression> comparison(void);
        std::shared_ptr<Expression> term(void);
        std::shared_ptr<Expression> factor(void);
        std::shared_ptr<Expression> unary(void);
        std::shared_ptr<Expression> primary(void);

    public:
        Parser(std::vector<Token> tokens);
        ~Parser(void);

        std::vector<std::shared_ptr<const Statement>> parse(void);
    };
}

#endif