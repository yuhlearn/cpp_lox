#ifndef _INTERPRETER_HPP
#define _INTERPRETER_HPP

#include <ast/expression.hpp>
#include <ast/statement.hpp>
#include <environment/environment.hpp>
#include <ast/value.hpp>
#include <exception>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <any>

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
        std::any evaluate(std::shared_ptr<Environment> env, std::shared_ptr<const Expression> expr) const;
        bool isTruthy(const Value &literal) const;
        bool isEqual(const Value &left, const Value &right) const;
        static std::string stringify(const Value &value);
        void checkNumberOperand(const Token &token, const Value &right) const;
        void checkNumberOperands(const Token &token, const Value &left, const Value &right) const;
        std::any lookUpVariable(std::shared_ptr<Environment> env,
                                std::shared_ptr<const Token> name,
                                std::shared_ptr<const Expression> expr) const;

    public:
        const std::shared_ptr<Environment> environment;
        const std::shared_ptr<Environment> globals;
        const std::shared_ptr<std::unordered_map<std::shared_ptr<const Expression>, int>> locals;

        Interpreter(void);
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
        void executeBlock(std::shared_ptr<Environment> env, std::shared_ptr<std::list<std::shared_ptr<Statement>>> statements) const;
        void execute(std::shared_ptr<Environment> env, std::shared_ptr<const Statement> stmt) const;
        void resolve(std::shared_ptr<Environment> env, std::shared_ptr<const Expression> expr, int depth) const;
        void interpret(std::vector<std::shared_ptr<const Statement>> &statements);
    };
}

#endif