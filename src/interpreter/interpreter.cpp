#include <interpreter/interpreter.hpp>
#include <ast/callable.hpp>
#include <ast/primitive.hpp>
#include <ast/function.hpp>
#include <ast/return_value.hpp>
#include <repl/repl.hpp>
#include <iostream>

using namespace Lox;
using namespace std;

Interpreter::Interpreter() : globals(make_shared<Environment>())
{
    auto clock = make_shared<LoxPrimitive>(LoxPrimitiveFn::clock);
    globals->define("clock", Value(ValueType::PRIMITIVE, clock));
}

/* 
PRIVATE 
*/

boost::any Interpreter::evaluate(shared_ptr<Environment> env, std::shared_ptr<const Expression> expr) const
{
    return expr->accept(env, *this);
}

bool Interpreter::isTruthy(const Value &literal) const
{
    if (literal.type == ValueType::NIL)
        return false;

    if (literal.type == ValueType::BOOLEAN)
        return boost::any_cast<bool>(literal.value);

    return true;
}

bool Interpreter::isEqual(const Value &left, const Value &right) const
{
    if (left.type == right.type)
    {
        switch (left.type)
        {
        case ValueType::NIL:
            return true;
        case ValueType::BOOLEAN:
            return boost::any_cast<bool>(left.value) == boost::any_cast<bool>(right.value);
        case ValueType::NUMBER:
            return boost::any_cast<double>(left.value) == boost::any_cast<double>(right.value);
        case ValueType::STRING:
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
    case ValueType::FUNCTION:
        return boost::any_cast<shared_ptr<LoxFunction>>(value.value)->toString();
    case ValueType::PRIMITIVE:
        return boost::any_cast<shared_ptr<LoxPrimitive>>(value.value)->toString();
    case ValueType::BOOLEAN:
        return boost::any_cast<bool>(value.value)
                   ? (string) "true"
                   : (string) "false";
    case ValueType::NIL:
        return (string) "nil";
    case ValueType::NUMBER:
        return std::to_string(boost::any_cast<double>(value.value));
    case ValueType::STRING:
        return boost::any_cast<string>(value.value);
    default:
        return "?";
    }
}

void Interpreter::checkNumberOperand(const Token &token, const Value &right) const
{
    if (ValueType::NUMBER == right.type)
        return;
    throw RuntimeError(token, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token &token, const Value &left, const Value &right) const
{
    if (ValueType::NUMBER == left.type && ValueType::NUMBER == right.type)
        return;
    throw RuntimeError(token, "Operands must be numbers.");
}

/* 
EXPRESSIONS 
*/

boost::any Interpreter::visitAssignExpression(shared_ptr<Environment> env, shared_ptr<const Assign> expr) const
{
    boost::any value = evaluate(env, expr->value);

    env->assign(*(expr->name), value);

    return value;
}

boost::any Interpreter::visitBinaryExpression(shared_ptr<Environment> env, shared_ptr<const Binary> expr) const
{
    auto left = boost::any_cast<Value>(evaluate(env, expr->left));
    auto right = boost::any_cast<Value>(evaluate(env, expr->right));

    switch (expr->op->type)
    {
    case TokenType::GREATER:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            boost::any_cast<double>(left.value) > boost::any_cast<double>(right.value));

    case TokenType::GREATER_EQUAL:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            boost::any_cast<double>(left.value) >= boost::any_cast<double>(right.value));

    case TokenType::LESS:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            boost::any_cast<double>(left.value) < boost::any_cast<double>(right.value));

    case TokenType::LESS_EQUAL:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            boost::any_cast<double>(left.value) <= boost::any_cast<double>(right.value));

    case TokenType::BANG_EQUAL:
        return Value(ValueType::BOOLEAN, !isEqual(left, right));

    case TokenType::EQUAL_EQUAL:
        return Value(ValueType::BOOLEAN, isEqual(left, right));

    case TokenType::MINUS:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::NUMBER,
            boost::any_cast<double>(left.value) - boost::any_cast<double>(right.value));

    case TokenType::PLUS:
        if (left.type == ValueType::NUMBER && right.type == ValueType::NUMBER)
            return Value(
                ValueType::NUMBER,
                boost::any_cast<double>(left.value) + boost::any_cast<double>(right.value));

        if (left.type == ValueType::STRING && right.type == ValueType::STRING)
            return Value(
                ValueType::STRING,
                boost::any_cast<string>(left.value) + boost::any_cast<string>(right.value));

        throw RuntimeError(*(expr->op), "Operands must be two numbers or two strings.");

    case TokenType::SLASH:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::NUMBER,
            boost::any_cast<double>(left.value) / boost::any_cast<double>(right.value));

    case TokenType::STAR:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::NUMBER,
            boost::any_cast<double>(left.value) * boost::any_cast<double>(right.value));

    default:
        return Value(ValueType::NIL, nullptr);
    }
}

boost::any Interpreter::visitCallExpression(shared_ptr<Environment> env, shared_ptr<const Call> expr) const
{
    auto callee = boost::any_cast<Value>(evaluate(env, expr->callee));
    auto arguments = make_shared<list<Value>>();

    for (auto arg : *expr->arguments)
        arguments->push_back(boost::any_cast<Value>(evaluate(env, arg)));

    if (callee.type == ValueType::PRIMITIVE)
    {
        auto function = boost::any_cast<shared_ptr<LoxPrimitive>>(callee.value);

        if (arguments->size() != function->arity())
            throw RuntimeError(*(expr->paren),
                               "Expected " + to_string(function->arity()) +
                                   " arguments but got " + to_string(arguments->size()) +
                                   ".");

        return function->call(*this, *arguments);
    }
    if (callee.type == ValueType::FUNCTION)
    {
        auto function = boost::any_cast<shared_ptr<LoxFunction>>(callee.value);

        if (arguments->size() != function->arity())
            throw RuntimeError(*(expr->paren),
                               "Expected " + to_string(function->arity()) +
                                   " arguments but got " + to_string(arguments->size()) +
                                   ".");

        return function->call(*this, *arguments);
    }

    throw RuntimeError(*(expr->paren), "Can only call functions and classes.");
}

boost::any Interpreter::visitGetExpression(shared_ptr<Environment> env, shared_ptr<const Get> expr) const
{
}

boost::any Interpreter::visitGroupingExpression(shared_ptr<Environment> env, shared_ptr<const Grouping> expr) const
{
    return evaluate(env, expr->expression);
}

boost::any Interpreter::visitLiteralExpression(shared_ptr<Environment> env, shared_ptr<const Literal> expr) const
{
    switch (*(expr->type))
    {
    case TokenType::IDENTIFIER:
        return Value(ValueType::IDENTIFIER, *(expr->value));
    case TokenType::STRING:
        return Value(ValueType::STRING, *(expr->value));
    case TokenType::NUMBER:
        return Value(ValueType::NUMBER, *(expr->value));
    case TokenType::BOOLEAN:
        return Value(ValueType::BOOLEAN, *(expr->value));
    default:
        return Value(ValueType::NIL, nullptr);
    }
}

boost::any Interpreter::visitLogicalExpression(shared_ptr<Environment> env, shared_ptr<const Logical> expr) const
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

boost::any Interpreter::visitSetExpression(shared_ptr<Environment> env, shared_ptr<const Set> expr) const {}
boost::any Interpreter::visitSuperExpression(shared_ptr<Environment> env, shared_ptr<const Super> expr) const {}
boost::any Interpreter::visitThisExpression(shared_ptr<Environment> env, shared_ptr<const This> expr) const {}

boost::any Interpreter::visitUnaryExpression(shared_ptr<Environment> env, shared_ptr<const Unary> expr) const
{
    auto right = boost::any_cast<Value>(evaluate(env, expr->right));

    switch (expr->op->type)
    {
    case TokenType::BANG:
        return Value(ValueType::BOOLEAN, !isTruthy(right));
    case TokenType::MINUS:
        checkNumberOperand(*(expr->op), right);
        return Value(right.type, -boost::any_cast<double>(right.value));
    default:
        return Value(ValueType::NIL, nullptr);
    }
}

boost::any Interpreter::visitVariableExpression(shared_ptr<Environment> env, shared_ptr<const Variable> expr) const
{
    return env->get(*(expr->name));
}

/* 
STATEMENTS 
*/

boost::any Interpreter::visitBlockStatement(shared_ptr<Environment> env, shared_ptr<const Block> stmt) const
{
    shared_ptr<Environment> new_env = make_shared<Environment>(env);

    executeBlock(new_env, stmt->statements);

    return nullptr;
}

boost::any Interpreter::visitClassStatement(shared_ptr<Environment> env, shared_ptr<const Class> stmt) const
{
}

boost::any Interpreter::visitExpressionStatementStatement(shared_ptr<Environment> env, shared_ptr<const ExpressionStatement> stmt) const
{
    evaluate(env, stmt->expression);

    return nullptr;
}

boost::any Interpreter::visitFunctionStatement(shared_ptr<Environment> env, shared_ptr<const Function> stmt) const
{
    shared_ptr<LoxFunction> function = make_shared<LoxFunction>(stmt, env);

    env->define(stmt->name->lexeme, Value(ValueType::FUNCTION, function));

    return nullptr;
}

boost::any Interpreter::visitIfStatement(shared_ptr<Environment> env, shared_ptr<const If> stmt) const
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

boost::any Interpreter::visitPrintStatement(shared_ptr<Environment> env, shared_ptr<const Print> stmt) const
{
    Value value = boost::any_cast<Value>(evaluate(env, stmt->expression));

    cout << stringify(value) << endl;

    return nullptr;
}

boost::any Interpreter::visitReturnStatement(shared_ptr<Environment> env, shared_ptr<const Return> stmt) const
{
    if (stmt->value != nullptr)
    {
        Value value = boost::any_cast<Value>(evaluate(env, stmt->value));
        throw ReturnValue(value);
    }

    throw ReturnValue(Value(ValueType::NIL, nullptr));
}

boost::any Interpreter::visitVarStatement(shared_ptr<Environment> env, shared_ptr<const Var> stmt) const
{
    boost::any value;

    if (stmt->initializer != nullptr)
        value = evaluate(env, stmt->initializer);
    else
        value = Value(ValueType::NIL, nullptr);

    env->define(stmt->name->lexeme, value);

    return nullptr;
}

boost::any Interpreter::visitWhileStatement(shared_ptr<Environment> env, shared_ptr<const While> stmt) const
{
    while (isTruthy(boost::any_cast<Value>(evaluate(env, stmt->condition))))
        execute(env, stmt->body);

    return nullptr;
}

/* 
OTHER 
*/

void Interpreter::execute(shared_ptr<Environment> env, shared_ptr<const Statement> stmt) const
{
    stmt->accept(env, *this);
}

void Interpreter::executeBlock(shared_ptr<Environment> env, shared_ptr<list<shared_ptr<Statement>>> statements) const
{
    for (auto statement : *statements)
        execute(env, statement);
}

void Interpreter::interpret(vector<shared_ptr<const Statement>> statements)
{
    try
    {
        for (shared_ptr<const Statement> stmt : statements)
            execute(this->globals, stmt);
    }
    catch (RuntimeError &error)
    {
        REPL::runtimeError(error);
    }
}
