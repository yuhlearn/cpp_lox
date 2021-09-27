#include <interpreter/interpreter.hpp>
#include <repl/repl.hpp>
#include <iostream>

using namespace Lox;
using namespace std;

/* 
PRIVATE 
*/

boost::any Interpreter::evaluate(std::shared_ptr<const Expression> expr) const
{
    return expr->accept(*this);
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
        return "\"" + boost::any_cast<string>(value.value) + "\"";
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

boost::any Interpreter::visitAssignExpression(shared_ptr<const Assign> expr) const {}

boost::any Interpreter::visitBinaryExpression(shared_ptr<const Binary> expr) const
{
    auto left = boost::any_cast<Value>(evaluate(expr->left));
    auto right = boost::any_cast<Value>(evaluate(expr->right));

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
        return NULL;
    }
}

boost::any Interpreter::visitCallExpression(shared_ptr<const Call> expr) const {}
boost::any Interpreter::visitGetExpression(shared_ptr<const Get> expr) const {}

boost::any Interpreter::visitGroupingExpression(shared_ptr<const Grouping> expr) const
{
    return evaluate(expr->expression);
}

boost::any Interpreter::visitLiteralExpression(shared_ptr<const Literal> expr) const
{
    return Value(*(expr->type), *(expr->value));
}

boost::any Interpreter::visitLogicalExpression(shared_ptr<const Logical> expr) const {}
boost::any Interpreter::visitSetExpression(shared_ptr<const Set> expr) const {}
boost::any Interpreter::visitSuperExpression(shared_ptr<const Super> expr) const {}
boost::any Interpreter::visitThisExpression(shared_ptr<const This> expr) const {}

boost::any Interpreter::visitUnaryExpression(shared_ptr<const Unary> expr) const
{
    auto right = boost::any_cast<Value>(evaluate(expr->right));

    switch (expr->op->type)
    {
    case TokenType::BANG:
        return Value(TokenType::BOOLEAN, !isTruthy(right));
    case TokenType::MINUS:
        checkNumberOperand(*(expr->op), right);
        return Value(right.type, -boost::any_cast<double>(right.value));
    default:
        return NULL;
    }
}

boost::any Interpreter::visitVariableExpression(shared_ptr<const Variable> expr) const
{
    return environment->get(*(expr->name));
}

/* 
STATEMENTS 
*/

boost::any Interpreter::visitBlockStatement(shared_ptr<const Block> stmt) const {}
boost::any Interpreter::visitClassStatement(shared_ptr<const Class> stmt) const {}

boost::any Interpreter::visitExpressionStatementStatement(shared_ptr<const ExpressionStatement> stmt) const
{
    evaluate(stmt->expression);

    return nullptr;
}

boost::any Interpreter::visitFunctionStatement(shared_ptr<const Function> stmt) const {}
boost::any Interpreter::visitIfStatement(shared_ptr<const If> stmt) const {}

boost::any Interpreter::visitPrintStatement(shared_ptr<const Print> stmt) const
{
    Value value = boost::any_cast<Value>(evaluate(stmt->expression));

    cout << stringify(value) << endl;

    return nullptr;
}

boost::any Interpreter::visitReturnStatement(shared_ptr<const Return> stmt) const {}

boost::any Interpreter::visitVarStatement(shared_ptr<const Var> stmt) const
{
    boost::any value = nullptr;

    if (stmt->initializer != nullptr)
        value = evaluate(stmt->initializer);

    environment->define(stmt->name->lexeme, value);

    return nullptr;
}

boost::any Interpreter::visitWhileStatement(shared_ptr<const While> stmt) const {}

/* 
OTHER 
*/

void Interpreter::execute(shared_ptr<const Statement> stmt)
{
    stmt->accept(*this);
}

void Interpreter::interpret(vector<shared_ptr<const Statement>> statements)
{
    try
    {
        for (shared_ptr<const Statement> stmt : statements)
            execute(stmt);
    }
    catch (RuntimeError &error)
    {
        REPL::runtimeError(error);
    }
}
