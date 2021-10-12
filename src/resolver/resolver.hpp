#ifndef _RESOLVER_HPP
#define _RESOLVER_HPP

#include <ast/expression.hpp>
#include <ast/statement.hpp>
#include <interpreter/interpreter.hpp>
#include <environment/environment.hpp>
#include <any>
#include <memory>
#include <deque>
#include <unordered_map>
#include <string>

namespace Lox
{
    enum FunctionType
    {
        NONE,
        FUNCTION
    };

    class Resolver : public ExpressionVisitor,
                     public StatementVisitor
    {
    private:
        const Interpreter &interpreter;

        const std::shared_ptr<std::deque<std::unordered_map<std::string, bool>>> scopes;
        std::unique_ptr<FunctionType> currentFunction;

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
        void resolveFunction(std::shared_ptr<Environment> env,
                             std::shared_ptr<const Function> function,
                             const FunctionType type) const;

    public:
        Resolver(Interpreter &interpreter);

        // EXPRESSIONS
        std::any visitAssignExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Assign> expr) const override;
        std::any visitBinaryExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Binary> expr) const override;
        std::any visitCallExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Call> expr) const override;
        std::any visitGetExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Get> expr) const override;
        std::any visitGroupingExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Grouping> expr) const override;
        std::any visitLiteralExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Literal> expr) const override;
        std::any visitLogicalExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Logical> expr) const override;
        std::any visitSetExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Set> expr) const override;
        std::any visitSuperExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Super> expr) const override;
        std::any visitThisExpression(std::shared_ptr<Environment> env, std::shared_ptr<const This> expr) const override;
        std::any visitUnaryExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Unary> expr) const override;
        std::any visitVariableExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Variable> expr) const override;

        // STATEMENTS
        std::any visitBlockStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Block> stmt) const override;
        std::any visitClassStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Class> stmt) const override;
        std::any visitExpressionStatementStatement(std::shared_ptr<Environment> env, std::shared_ptr<const ExpressionStatement> stmt) const override;
        std::any visitFunctionStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Function> stmt) const override;
        std::any visitIfStatement(std::shared_ptr<Environment> env, std::shared_ptr<const If> stmt) const override;
        std::any visitPrintStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Print> stmt) const override;
        std::any visitReturnStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Return> stmt) const override;
        std::any visitVarStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Var> stmt) const override;
        std::any visitWhileStatement(std::shared_ptr<Environment> env, std::shared_ptr<const While> stmt) const override;

        // OTHER
        void resolve(std::shared_ptr<Environment> env, std::vector<std::shared_ptr<const Statement>> &statements) const;
    };
}

#endif