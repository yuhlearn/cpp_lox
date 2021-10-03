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
        boost::any evaluate(Environment &env, std::shared_ptr<const Expression> expr) const;
        bool isTruthy(const Value &literal) const;
        bool isEqual(const Value &left, const Value &right) const;
        static std::string stringify(const Value &value);
        void checkNumberOperand(const Token &token, const Value &right) const;
        void checkNumberOperands(const Token &token, const Value &left, const Value &right) const;
        void executeBlock(Environment &env, std::shared_ptr<std::list<std::shared_ptr<Statement>>> statements) const;
        void execute(Environment &env, std::shared_ptr<const Statement> stmt) const;

    public:
        Interpreter() {}
        // EXPRESSIONS
        boost::any visitAssignExpression(Environment &env, std::shared_ptr<const Assign> expr) const override;
        boost::any visitBinaryExpression(Environment &env, std::shared_ptr<const Binary> expr) const override;
        boost::any visitCallExpression(Environment &env, std::shared_ptr<const Call> expr) const override;
        boost::any visitGetExpression(Environment &env, std::shared_ptr<const Get> expr) const override;
        boost::any visitGroupingExpression(Environment &env, std::shared_ptr<const Grouping> expr) const override;
        boost::any visitLiteralExpression(Environment &env, std::shared_ptr<const Literal> expr) const override;
        boost::any visitLogicalExpression(Environment &env, std::shared_ptr<const Logical> expr) const override;
        boost::any visitSetExpression(Environment &env, std::shared_ptr<const Set> expr) const override;
        boost::any visitSuperExpression(Environment &env, std::shared_ptr<const Super> expr) const override;
        boost::any visitThisExpression(Environment &env, std::shared_ptr<const This> expr) const override;
        boost::any visitUnaryExpression(Environment &env, std::shared_ptr<const Unary> expr) const override;
        boost::any visitVariableExpression(Environment &env, std::shared_ptr<const Variable> expr) const override;
        // STATEMENTS
        boost::any visitBlockStatement(Environment &env, std::shared_ptr<const Block> stmt) const override;
        boost::any visitClassStatement(Environment &env, std::shared_ptr<const Class> stmt) const override;
        boost::any visitExpressionStatementStatement(Environment &env, std::shared_ptr<const ExpressionStatement> stmt) const override;
        boost::any visitFunctionStatement(Environment &env, std::shared_ptr<const Function> stmt) const override;
        boost::any visitIfStatement(Environment &env, std::shared_ptr<const If> stmt) const override;
        boost::any visitPrintStatement(Environment &env, std::shared_ptr<const Print> stmt) const override;
        boost::any visitReturnStatement(Environment &env, std::shared_ptr<const Return> stmt) const override;
        boost::any visitVarStatement(Environment &env, std::shared_ptr<const Var> stmt) const override;
        boost::any visitWhileStatement(Environment &env, std::shared_ptr<const While> stmt) const override;
        // OTHER
        void interpret(Environment &env, std::vector<std::shared_ptr<const Statement>> statements);
    };
}

#endif