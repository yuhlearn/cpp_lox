#include <interpreter/interpreter.hpp>
#include <repl/repl.hpp>
#include <iostream>

using namespace Lox;
using namespace std;

/* 
PRIVATE 
*/

boost::any Interpreter::evaluate(Environment &env, std::shared_ptr<const Expression> expr) const
{
    return expr->accept(env, *this);
}

bool Interpreter::isTruthy(const Value &literal) const
{
    if (literal.type == TokenType::NIL)
        return false;

    if (literal.type == TokenType::BOOLEAN)
        return boost::any_cast<bool>(literal.value);

    return true;
}

bool Interpreter::isEqual(const Value &left, const Value &right) const
{
    if (left.type == right.type)
    {
        switch (left.type)
        {
        case TokenType::NIL:
            return true;
        case TokenType::BOOLEAN:
            return boost::any_cast<bool>(left.value) == boost::any_cast<bool>(right.value);
        case TokenType::NUMBER:
            return boost::any_cast<double>(left.value) == boost::any_cast<double>(right.value);
        case TokenType::STRING:
            return boost::any_cast<string>(left.value) == boost::any_cast<string>(right.value);
        default:
            return false;
        }
    }
    return false;
}

std::string Interpreter::stringify(const Value &value)
{
    switch (value.type)
    {
    case TokenType::BOOLEAN:
        return boost::any_cast<bool>(value.value)
                   ? (string) "true"
                   : (string) "false";
    case TokenType::NIL:
        return (string) "nil";
    case TokenType::NUMBER:
        return std::to_string(boost::any_cast<double>(value.value));
    case TokenType::STRING:
        return boost::any_cast<string>(value.value);
    default:
        return "?";
    }
}

void Interpreter::checkNumberOperand(const Token &token, const Value &right) const
{
    if (TokenType::NUMBER == right.type)
        return;
    throw RuntimeError(token, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token &token, const Value &left, const Value &right) const
{
    if (TokenType::NUMBER == left.type && TokenType::NUMBER == right.type)
        return;
    throw RuntimeError(token, "Operands must be numbers.");
}

/* 
EXPRESSIONS 
*/

boost::any Interpreter::visitAssignExpression(Environment &env, shared_ptr<const Assign> expr) const
{
    boost::any value = evaluate(env, expr->value);

    env.assign(*(expr->name), value);

    return value;
}

boost::any Interpreter::visitBinaryExpression(Environment &env, shared_ptr<const Binary> expr) const
{
    auto left = boost::any_cast<Value>(evaluate(env, expr->left));
    auto right = boost::any_cast<Value>(evaluate(env, expr->right));

    switch (expr->op->type)
    {
    case TokenType::GREATER:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            TokenType::BOOLEAN,
            boost::any_cast<double>(left.value) > boost::any_cast<double>(right.value));

    case TokenType::GREATER_EQUAL:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            TokenType::BOOLEAN,
            boost::any_cast<double>(left.value) >= boost::any_cast<double>(right.value));

    case TokenType::LESS:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            TokenType::BOOLEAN,
            boost::any_cast<double>(left.value) < boost::any_cast<double>(right.value));

    case TokenType::LESS_EQUAL:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            TokenType::BOOLEAN,
            boost::any_cast<double>(left.value) <= boost::any_cast<double>(right.value));

    case TokenType::BANG_EQUAL:
        return Value(TokenType::BOOLEAN, !isEqual(left, right));

    case TokenType::EQUAL_EQUAL:
        return Value(TokenType::BOOLEAN, isEqual(left, right));

    case TokenType::MINUS:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            TokenType::NUMBER,
            boost::any_cast<double>(left.value) - boost::any_cast<double>(right.value));

    case TokenType::PLUS:
        if (left.type == TokenType::NUMBER && right.type == TokenType::NUMBER)
            return Value(
                TokenType::NUMBER,
                boost::any_cast<double>(left.value) + boost::any_cast<double>(right.value));

        if (left.type == TokenType::STRING && right.type == TokenType::STRING)
            return Value(
                TokenType::STRING,
                boost::any_cast<string>(left.value) + boost::any_cast<string>(right.value));

        throw RuntimeError(*(expr->op), "Operands must be two numbers or two strings.");

    case TokenType::SLASH:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            TokenType::NUMBER,
            boost::any_cast<double>(left.value) / boost::any_cast<double>(right.value));

    case TokenType::STAR:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            TokenType::NUMBER,
            boost::any_cast<double>(left.value) * boost::any_cast<double>(right.value));

    default:
        return Value(TokenType::NIL, nullptr);
    }
}

boost::any Interpreter::visitCallExpression(Environment &env, shared_ptr<const Call> expr) const {}
boost::any Interpreter::visitGetExpression(Environment &env, shared_ptr<const Get> expr) const {}

boost::any Interpreter::visitGroupingExpression(Environment &env, shared_ptr<const Grouping> expr) const
{
    return evaluate(env, expr->expression);
}

boost::any Interpreter::visitLiteralExpression(Environment &env, shared_ptr<const Literal> expr) const
{
    if (*(expr->type) != TokenType::NIL)
        return Value(*(expr->type), *(expr->value));
    return Value(*(expr->type), nullptr);
}

boost::any Interpreter::visitLogicalExpression(Environment &env, shared_ptr<const Logical> expr) const
{
    auto left = boost::any_cast<Value>(evaluate(env, expr->left));

    if (expr->op->type == TokenType::OR)
    {
        if (isTruthy(left))
            return left;
    }
    else
    {
        if (!isTruthy(left))
            return left;
    }

    return evaluate(env, expr->right);
}

boost::any Interpreter::visitSetExpression(Environment &env, shared_ptr<const Set> expr) const {}
boost::any Interpreter::visitSuperExpression(Environment &env, shared_ptr<const Super> expr) const {}
boost::any Interpreter::visitThisExpression(Environment &env, shared_ptr<const This> expr) const {}

boost::any Interpreter::visitUnaryExpression(Environment &env, shared_ptr<const Unary> expr) const
{
    auto right = boost::any_cast<Value>(evaluate(env, expr->right));

    switch (expr->op->type)
    {
    case TokenType::BANG:
        return Value(TokenType::BOOLEAN, !isTruthy(right));
    case TokenType::MINUS:
        checkNumberOperand(*(expr->op), right);
        return Value(right.type, -boost::any_cast<double>(right.value));
    default:
        return Value(TokenType::NIL, nullptr);
    }
}

boost::any Interpreter::visitVariableExpression(Environment &env, shared_ptr<const Variable> expr) const
{
    return env.get(*(expr->name));
}

/* 
STATEMENTS 
*/

boost::any Interpreter::visitBlockStatement(Environment &env, shared_ptr<const Block> stmt) const
{
    Environment new_env = Environment(&env);

    executeBlock(new_env, stmt->statements);

    return nullptr;
}

boost::any Interpreter::visitClassStatement(Environment &env, shared_ptr<const Class> stmt) const
{
}

boost::any Interpreter::visitExpressionStatementStatement(Environment &env, shared_ptr<const ExpressionStatement> stmt) const
{
    evaluate(env, stmt->expression);

    return nullptr;
}

boost::any Interpreter::visitFunctionStatement(Environment &env, shared_ptr<const Function> stmt) const {}

boost::any Interpreter::visitIfStatement(Environment &env, shared_ptr<const If> stmt) const
{
    auto value = boost::any_cast<Value>(evaluate(env, stmt->condition));

    if (isTruthy(value))
    {
        execute(env, stmt->thenBranch);
    }
    else if (stmt->elseBranch != nullptr)
    {
        execute(env, stmt->elseBranch);
    }

    return nullptr;
}

boost::any Interpreter::visitPrintStatement(Environment &env, shared_ptr<const Print> stmt) const
{
    Value value = boost::any_cast<Value>(evaluate(env, stmt->expression));

    cout << stringify(value) << endl;

    return nullptr;
}

boost::any Interpreter::visitReturnStatement(Environment &env, shared_ptr<const Return> stmt) const {}

boost::any Interpreter::visitVarStatement(Environment &env, shared_ptr<const Var> stmt) const
{
    boost::any value;

    if (stmt->initializer != nullptr)
        value = evaluate(env, stmt->initializer);
    else
        value = Value(TokenType::NIL, nullptr);

    env.define(stmt->name->lexeme, value);

    return nullptr;
}

boost::any Interpreter::visitWhileStatement(Environment &env, shared_ptr<const While> stmt) const
{
    while (isTruthy(boost::any_cast<Value>(evaluate(env, stmt->condition))))
        execute(env, stmt->body);

    return nullptr;
}

/* 
OTHER 
*/

void Interpreter::execute(Environment &env, shared_ptr<const Statement> stmt) const
{
    stmt->accept(env, *this);
}

void Interpreter::executeBlock(Environment &env, shared_ptr<list<shared_ptr<Statement>>> statements) const
{
    for (auto statement : *statements)
        execute(env, statement);
}

void Interpreter::interpret(Environment &env, vector<shared_ptr<const Statement>> statements)
{
    try
    {
        for (shared_ptr<const Statement> stmt : statements)
            execute(env, stmt);
    }
    catch (RuntimeError &error)
    {
        REPL::runtimeError(error);
    }
}
