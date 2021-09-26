#ifndef _INTERPRETER_HPP
#define _INTERPRETER_HPP

#include <ast/expression.hpp>
#include <string>
#include <memory>
#include <boost/any.hpp>

namespace Lox
{
    class RuntimeError : public std::exception
    {
    public:
        const Token token;
        const std::string message;

        RuntimeError(Token token, std::string message) : token(token), message(message)
        {
        }
    };

    class Interpreter : public ExpressionVisitor
    {
    private:
        boost::any evaluate(std::shared_ptr<const Expression> expr) const;
        bool isTruthy(const Literal &literal) const;
        bool isEqual(const Literal &left, const Literal &right) const;
        std::string stringify(const Literal &literal);
        void checkNumberOperand(const Token &token, const Literal &right) const;
        void checkNumberOperands(const Token &token, const Literal &left, const Literal &right) const;

    public:
        boost::any visitAssignExpression(std::shared_ptr<const Assign> expr) const override;
        boost::any visitBinaryExpression(std::shared_ptr<const Binary> expr) const override;
        boost::any visitCallExpression(std::shared_ptr<const Call> expr) const override;
        boost::any visitGetExpression(std::shared_ptr<const Get> expr) const override;
        boost::any visitGroupingExpression(std::shared_ptr<const Grouping> expr) const override;
        boost::any visitLiteralExpression(std::shared_ptr<const Literal> expr) const override;
        boost::any visitLogicalExpression(std::shared_ptr<const Logical> expr) const override;
        boost::any visitSetExpression(std::shared_ptr<const Set> expr) const override;
        boost::any visitSuperExpression(std::shared_ptr<const Super> expr) const override;
        boost::any visitThisExpression(std::shared_ptr<const This> expr) const override;
        boost::any visitUnaryExpression(std::shared_ptr<const Unary> expr) const override;
        boost::any visitVariableExpression(std::shared_ptr<const Variable> expr) const override;
        void interpret(std::shared_ptr<Expression> expr);
    };
}

#endif