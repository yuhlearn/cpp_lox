#ifndef _RESOLVER_HPP
#define _RESOLVER_HPP

#include <ast/expression.hpp>
#include <ast/statement.hpp>
#include <interpreter/interpreter.hpp>
#include <environment/environment.hpp>
#include <boost/any.hpp>
#include <memory>
#include <deque>
#include <unordered_map>
#include <string>

namespace Lox
{
    class Resolver : public ExpressionVisitor, public StatementVisitor
    {
    private:
        const Interpreter &interpreter;

        const std::unique_ptr<std::deque<std::unordered_map<std::string, bool>>> scopes;

        void define(std::shared_ptr<const Token> name) const;
        void declare(std::shared_ptr<const Token> name) const;
        void beginScope(void) const;
        void endScope(void) const;
        void resolve(std::shared_ptr<Environment> env, std::shared_ptr<const Expression> expr) const;
        void resolve(std::shared_ptr<Environment> env, std::shared_ptr<const Statement> stmt) const;
        void resolve(std::shared_ptr<Environment> env, std::shared_ptr<std::list<std::shared_ptr<Statement>>> statements) const;
        void resolveLocal(std::shared_ptr<Environment> env,
                          std::shared_ptr<const Expression> expr,
                          std::shared_ptr<const Token> name) const;
        void resolveFunction(std::shared_ptr<Environment> env, std::shared_ptr<const Function> function) const;

    public:
        Resolver(Interpreter &Interpreter);

        // EXPRESSIONS
        boost::any visitAssignExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Assign> expr) const override;
        boost::any visitBinaryExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Binary> expr) const override;
        boost::any visitCallExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Call> expr) const override;
        boost::any visitGetExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Get> expr) const override;
        boost::any visitGroupingExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Grouping> expr) const override;
        boost::any visitLiteralExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Literal> expr) const override;
        boost::any visitLogicalExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Logical> expr) const override;
        boost::any visitSetExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Set> expr) const override;
        boost::any visitSuperExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Super> expr) const override;
        boost::any visitThisExpression(std::shared_ptr<Environment> env, std::shared_ptr<const This> expr) const override;
        boost::any visitUnaryExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Unary> expr) const override;
        boost::any visitVariableExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Variable> expr) const override;

        // STATEMENTS
        boost::any visitBlockStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Block> stmt) const override;
        boost::any visitClassStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Class> stmt) const override;
        boost::any visitExpressionStatementStatement(std::shared_ptr<Environment> env, std::shared_ptr<const ExpressionStatement> stmt) const override;
        boost::any visitFunctionStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Function> stmt) const override;
        boost::any visitIfStatement(std::shared_ptr<Environment> env, std::shared_ptr<const If> stmt) const override;
        boost::any visitPrintStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Print> stmt) const override;
        boost::any visitReturnStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Return> stmt) const override;
        boost::any visitVarStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Var> stmt) const override;
        boost::any visitWhileStatement(std::shared_ptr<Environment> env, std::shared_ptr<const While> stmt) const override;
    };
}

#endif