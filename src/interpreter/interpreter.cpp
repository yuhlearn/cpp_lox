#include <interpreter/interpreter.hpp>
#include <ast/callable.hpp>
#include <ast/primitive.hpp>
#include <ast/function.hpp>
#include <ast/return_value.hpp>
#include <repl/repl.hpp>
#include <iostream>

using namespace Lox;
using namespace std;

Interpreter::Interpreter()
    : environment(make_shared<Environment>()),
      globals(make_shared<Environment>()),
      locals(make_shared<unordered_map<shared_ptr<const Expression>, int>>())

{
    auto clock = make_shared<LoxPrimitive>(LoxPrimitiveFn::clock);
    globals->define("clock", Value(ValueType::PRIMITIVE, clock));
}

/* 
PRIVATE 
*/

std::any Interpreter::evaluate(shared_ptr<Environment> env, std::shared_ptr<const Expression> expr) const
{
    return expr->accept(env, *this);
}

bool Interpreter::isTruthy(const Value &literal) const
{
    if (literal.type == ValueType::NIL)
        return false;

    if (literal.type == ValueType::BOOLEAN)
        return std::any_cast<bool>(literal.value);

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
            return std::any_cast<bool>(left.value) == std::any_cast<bool>(right.value);
        case ValueType::NUMBER:
            return std::any_cast<double>(left.value) == std::any_cast<double>(right.value);
        case ValueType::STRING:
            return std::any_cast<string>(left.value) == std::any_cast<string>(right.value);
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
        return std::any_cast<shared_ptr<LoxFunction>>(value.value)->toString();
    case ValueType::PRIMITIVE:
        return std::any_cast<shared_ptr<LoxPrimitive>>(value.value)->toString();
    case ValueType::BOOLEAN:
        return std::any_cast<bool>(value.value)
                   ? (string) "true"
                   : (string) "false";
    case ValueType::NIL:
        return (string) "nil";
    case ValueType::NUMBER:
        return std::to_string(std::any_cast<double>(value.value));
    case ValueType::STRING:
        return std::any_cast<string>(value.value);
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

std::any Interpreter::lookUpVariable(std::shared_ptr<Environment> env,
                                     std::shared_ptr<const Token> name,
                                     std::shared_ptr<const Expression> expr) const
{
    auto distance = locals->find(expr);

    if (distance != locals->end())
    {
        return env->getAt(distance->second, *name);
    }
    else
    {
        return globals->get(*name);
    }
}

/* 
EXPRESSIONS 
*/

std::any Interpreter::visitAssignExpression(shared_ptr<Environment> env, shared_ptr<const Assign> expr) const
{
    std::any value = evaluate(env, expr->value);

    auto distance = locals->find(expr);

    if (distance != locals->end())
    {
        env->assignAt(distance->second, *(expr->name), value);
    }
    else
    {
        globals->assign(*(expr->name), value);
    }

    return value;
}

std::any Interpreter::visitBinaryExpression(shared_ptr<Environment> env, shared_ptr<const Binary> expr) const
{
    auto left = std::any_cast<Value>(evaluate(env, expr->left));
    auto right = std::any_cast<Value>(evaluate(env, expr->right));

    switch (expr->op->type)
    {
    case TokenType::GREATER:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            std::any_cast<double>(left.value) > std::any_cast<double>(right.value));

    case TokenType::GREATER_EQUAL:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            std::any_cast<double>(left.value) >= std::any_cast<double>(right.value));

    case TokenType::LESS:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            std::any_cast<double>(left.value) < std::any_cast<double>(right.value));

    case TokenType::LESS_EQUAL:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::BOOLEAN,
            std::any_cast<double>(left.value) <= std::any_cast<double>(right.value));

    case TokenType::BANG_EQUAL:
        return Value(ValueType::BOOLEAN, !isEqual(left, right));

    case TokenType::EQUAL_EQUAL:
        return Value(ValueType::BOOLEAN, isEqual(left, right));

    case TokenType::MINUS:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::NUMBER,
            std::any_cast<double>(left.value) - std::any_cast<double>(right.value));

    case TokenType::PLUS:
        if (left.type == ValueType::NUMBER && right.type == ValueType::NUMBER)
            return Value(
                ValueType::NUMBER,
                std::any_cast<double>(left.value) + std::any_cast<double>(right.value));

        if (left.type == ValueType::STRING && right.type == ValueType::STRING)
            return Value(
                ValueType::STRING,
                std::any_cast<string>(left.value) + std::any_cast<string>(right.value));

        throw RuntimeError(*(expr->op), "Operands must be two numbers or two strings.");

    case TokenType::SLASH:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::NUMBER,
            std::any_cast<double>(left.value) / std::any_cast<double>(right.value));

    case TokenType::STAR:
        checkNumberOperands(*(expr->op), left, right);
        return Value(
            ValueType::NUMBER,
            std::any_cast<double>(left.value) * std::any_cast<double>(right.value));

    default:
        return Value(ValueType::NIL, nullptr);
    }
}

std::any Interpreter::visitCallExpression(shared_ptr<Environment> env, shared_ptr<const Call> expr) const
{
    auto callee = std::any_cast<Value>(evaluate(env, expr->callee));
    auto arguments = make_shared<list<Value>>();

    for (auto arg : *expr->arguments)
        arguments->push_back(std::any_cast<Value>(evaluate(env, arg)));

    if (callee.type == ValueType::PRIMITIVE)
    {
        auto function = std::any_cast<shared_ptr<LoxPrimitive>>(callee.value);

        if (arguments->size() != function->arity())
            throw RuntimeError(*(expr->paren),
                               "Expected " + to_string(function->arity()) +
                                   " arguments but got " + to_string(arguments->size()) +
                                   ".");

        return function->call(*this, *arguments);
    }
    if (callee.type == ValueType::FUNCTION)
    {
        auto function = std::any_cast<shared_ptr<LoxFunction>>(callee.value);

        if (arguments->size() != function->arity())
            throw RuntimeError(*(expr->paren),
                               "Expected " + to_string(function->arity()) +
                                   " arguments but got " + to_string(arguments->size()) +
                                   ".");

        return function->call(*this, *arguments);
    }

    throw RuntimeError(*(expr->paren), "Can only call functions and classes.");
}

std::any Interpreter::visitGetExpression(shared_ptr<Environment>, shared_ptr<const Get>) const
{
    return nullptr;
}

std::any Interpreter::visitGroupingExpression(shared_ptr<Environment> env, shared_ptr<const Grouping> expr) const
{
    return evaluate(env, expr->expression);
}

std::any Interpreter::visitLiteralExpression(shared_ptr<Environment>, shared_ptr<const Literal> expr) const
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

std::any Interpreter::visitLogicalExpression(shared_ptr<Environment> env, shared_ptr<const Logical> expr) const
{
    auto left = std::any_cast<Value>(evaluate(env, expr->left));

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

std::any Interpreter::visitSetExpression(shared_ptr<Environment>, shared_ptr<const Set>) const
{
    return nullptr;
}

std::any Interpreter::visitSuperExpression(shared_ptr<Environment>, shared_ptr<const Super>) const
{
    return nullptr;
}

std::any Interpreter::visitThisExpression(shared_ptr<Environment>, shared_ptr<const This>) const
{
    return nullptr;
}

std::any Interpreter::visitUnaryExpression(shared_ptr<Environment> env, shared_ptr<const Unary> expr) const
{
    auto right = std::any_cast<Value>(evaluate(env, expr->right));

    switch (expr->op->type)
    {
    case TokenType::BANG:
        return Value(ValueType::BOOLEAN, !isTruthy(right));
    case TokenType::MINUS:
        checkNumberOperand(*(expr->op), right);
        return Value(right.type, -std::any_cast<double>(right.value));
    default:
        return Value(ValueType::NIL, nullptr);
    }
}

std::any Interpreter::visitVariableExpression(shared_ptr<Environment> env, shared_ptr<const Variable> expr) const
{
    return lookUpVariable(env, expr->name, expr);
}

/* 
STATEMENTS 
*/

std::any Interpreter::visitBlockStatement(shared_ptr<Environment> env, shared_ptr<const Block> stmt) const
{
    shared_ptr<Environment> new_env = make_shared<Environment>(env);

    executeBlock(new_env, stmt->statements);

    return nullptr;
}

std::any Interpreter::visitClassStatement(shared_ptr<Environment>, shared_ptr<const Class>) const
{
    return nullptr;
}

std::any Interpreter::visitExpressionStatementStatement(shared_ptr<Environment> env, shared_ptr<const ExpressionStatement> stmt) const
{
    evaluate(env, stmt->expression);

    return nullptr;
}

std::any Interpreter::visitFunctionStatement(shared_ptr<Environment> env, shared_ptr<const Function> stmt) const
{
    shared_ptr<LoxFunction> function = make_shared<LoxFunction>(stmt, env);

    env->define(stmt->name->lexeme, Value(ValueType::FUNCTION, function));

    return nullptr;
}

std::any Interpreter::visitIfStatement(shared_ptr<Environment> env, shared_ptr<const If> stmt) const
{
    auto value = std::any_cast<Value>(evaluate(env, stmt->condition));

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

std::any Interpreter::visitPrintStatement(shared_ptr<Environment> env, shared_ptr<const Print> stmt) const
{
    Value value = std::any_cast<Value>(evaluate(env, stmt->expression));

    cout << stringify(value) << endl;

    return nullptr;
}

std::any Interpreter::visitReturnStatement(shared_ptr<Environment> env, shared_ptr<const Return> stmt) const
{
    if (stmt->value != nullptr)
    {
        Value value = std::any_cast<Value>(evaluate(env, stmt->value));
        throw ReturnValue(value);
    }

    throw ReturnValue(Value(ValueType::NIL, nullptr));
}

std::any Interpreter::visitVarStatement(shared_ptr<Environment> env, shared_ptr<const Var> stmt) const
{
    std::any value;

    if (stmt->initializer != nullptr)
        value = evaluate(env, stmt->initializer);
    else
        value = Value(ValueType::NIL, nullptr);

    env->define(stmt->name->lexeme, value);

    return nullptr;
}

std::any Interpreter::visitWhileStatement(shared_ptr<Environment> env, shared_ptr<const While> stmt) const
{
    while (isTruthy(std::any_cast<Value>(evaluate(env, stmt->condition))))
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

void Interpreter::resolve(std::shared_ptr<Environment>, std::shared_ptr<const Expression> expr, int depth) const
{
    (*locals)[expr] = depth; // ->insert(make_pair(expr, depth));
}

void Interpreter::interpret(vector<shared_ptr<const Statement>> &statements)
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
