#ifndef _INTERPRETER_HPP
#define _INTERPRETER_HPP

#include <ast/expression.hpp>
#include <ast/statement.hpp>
#include <environment/environment.hpp>
#include <ast/value.hpp>
#include <vector>
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

    class Interpreter : public ExpressionVisitor, public StatementVisitor
    {
    private:
        std::unique_ptr<Environment> environment;

        boost::any evaluate(std::shared_ptr<const Expression> expr) const;
        bool isTruthy(const Value &literal) const;
        bool isEqual(const Value &left, const Value &right) const;
        static std::string stringify(const Value &value);
        void checkNumberOperand(const Token &token, const Value &right) const;
        void checkNumberOperands(const Token &token, const Value &left, const Value &right) const;

    public:
        Interpreter() : environment(new Environment()) {}
        // EXPRESSIONS
        boost::any
        visitAssignExpression(std::shared_ptr<const Assign> expr) const override;
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
        // STATEMENTS
        boost::any visitBlockStatement(std::shared_ptr<const Block> stmt) const override;
        boost::any visitClassStatement(std::shared_ptr<const Class> stmt) const override;
        boost::any visitExpressionStatementStatement(std::shared_ptr<const ExpressionStatement> stmt) const override;
        boost::any visitFunctionStatement(std::shared_ptr<const Function> stmt) const override;
        boost::any visitIfStatement(std::shared_ptr<const If> stmt) const override;
        boost::any visitPrintStatement(std::shared_ptr<const Print> stmt) const override;
        boost::any visitReturnStatement(std::shared_ptr<const Return> stmt) const override;
        boost::any visitVarStatement(std::shared_ptr<const Var> stmt) const override;
        boost::any visitWhileStatement(std::shared_ptr<const While> stmt) const override;
        // OTHER
        void execute(std::shared_ptr<const Statement> stmt);

        void interpret(std::vector<std::shared_ptr<const Statement>> statements);
    };
}

#endif