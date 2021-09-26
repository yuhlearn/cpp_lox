#include <interpreter/interpreter.hpp>
#include <repl/repl.hpp>
#include <iostream>

using namespace Lox;
using namespace std;

boost::any Interpreter::evaluate(std::shared_ptr<const Expression> expr) const
{
    return expr->accept(*this);
}

bool Interpreter::isTruthy(const Literal &literal) const
{
    if (literal.type == TokenType::NIL)
        return false;

    if (literal.type == TokenType::BOOLEAN)
        return boost::any_cast<bool>(literal.value);

    return true;
}

bool Interpreter::isEqual(const Literal &left, const Literal &right) const
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

std::string Interpreter::stringify(const Literal &literal)
{
    switch (literal.type)
    {
    case TokenType::BOOLEAN:
        return boost::any_cast<bool>(literal.value)
                   ? (string) "true"
                   : (string) "false";
    case TokenType::NIL:
        return (string) "nil";
    case TokenType::NUMBER:
        return std::to_string(boost::any_cast<double>(literal.value));
    case TokenType::STRING:
        return "\"" + boost::any_cast<string>(literal.value) + "\"";
    default:
        return "?";
    }
}

void Interpreter::checkNumberOperand(const Token &token, const Literal &right) const
{
    if (TokenType::NUMBER == right.type)
        return;
    throw RuntimeError(token, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token &token, const Literal &left, const Literal &right) const
{
    if (TokenType::NUMBER == left.type && TokenType::NUMBER == right.type)
        return;
    throw RuntimeError(token, "Operands must be numbers.");
}

boost::any Interpreter::visitAssignExpression(shared_ptr<const Assign> expr) const {}

boost::any Interpreter::visitBinaryExpression(shared_ptr<const Binary> expr) const
{
    shared_ptr<const Literal> left = boost::any_cast<shared_ptr<const Literal>>(evaluate(expr->left));
    shared_ptr<const Literal> right = boost::any_cast<shared_ptr<const Literal>>(evaluate(expr->right));

    switch (expr->op->type)
    {
    case TokenType::GREATER:
        checkNumberOperands(*(expr->op), *left, *right);
        return make_shared<const Literal>(
            TokenType::BOOLEAN,
            boost::any_cast<bool>(left->value) > boost::any_cast<bool>(right->value));

    case TokenType::GREATER_EQUAL:
        checkNumberOperands(*(expr->op), *left, *right);
        return make_shared<const Literal>(
            TokenType::BOOLEAN,
            boost::any_cast<bool>(left->value) >= boost::any_cast<bool>(right->value));

    case TokenType::LESS:
        checkNumberOperands(*(expr->op), *left, *right);
        return make_shared<const Literal>(
            TokenType::BOOLEAN,
            boost::any_cast<bool>(left->value) < boost::any_cast<bool>(right->value));

    case TokenType::LESS_EQUAL:
        checkNumberOperands(*(expr->op), *left, *right);
        return make_shared<const Literal>(
            TokenType::BOOLEAN,
            boost::any_cast<bool>(left->value) <= boost::any_cast<bool>(right->value));

    case TokenType::BANG_EQUAL:
        return make_shared<const Literal>(TokenType::BOOLEAN, !isEqual(*left, *right));

    case TokenType::EQUAL_EQUAL:
        return make_shared<const Literal>(TokenType::BOOLEAN, isEqual(*left, *right));

    case TokenType::MINUS:
        checkNumberOperands(*(expr->op), *left, *right);
        return make_shared<const Literal>(
            TokenType::NUMBER,
            boost::any_cast<double>(left->value) - boost::any_cast<double>(right->value));

    case TokenType::PLUS:
        if (left->type == TokenType::NUMBER && right->type == TokenType::NUMBER)
            return make_shared<const Literal>(
                TokenType::NUMBER,
                boost::any_cast<double>(left->value) + boost::any_cast<double>(right->value));

        if (left->type == TokenType::STRING && right->type == TokenType::STRING)

            return make_shared<const Literal>(
                TokenType::STRING,
                boost::any_cast<string>(left->value) + boost::any_cast<string>(right->value));
        throw RuntimeError(*(expr->op), "Operands must be two numbers or two strings.");

    case TokenType::SLASH:
        checkNumberOperands(*(expr->op), *left, *right);
        return make_shared<const Literal>(
            TokenType::NUMBER,
            boost::any_cast<double>(left->value) / boost::any_cast<double>(right->value));

    case TokenType::STAR:
        checkNumberOperands(*(expr->op), *left, *right);
        return make_shared<const Literal>(
            TokenType::NUMBER,
            boost::any_cast<double>(left->value) * boost::any_cast<double>(right->value));

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
    return expr;
}

boost::any Interpreter::visitLogicalExpression(shared_ptr<const Logical> expr) const {}
boost::any Interpreter::visitSetExpression(shared_ptr<const Set> expr) const {}
boost::any Interpreter::visitSuperExpression(shared_ptr<const Super> expr) const {}
boost::any Interpreter::visitThisExpression(shared_ptr<const This> expr) const {}

boost::any Interpreter::visitUnaryExpression(shared_ptr<const Unary> expr) const
{
    shared_ptr<const Literal> right = boost::any_cast<shared_ptr<const Literal>>(evaluate(expr->right));

    switch (expr->op->type)
    {
    case TokenType::BANG:
        return make_shared<const Literal>(TokenType::BOOLEAN, !isTruthy(*right));
    case TokenType::MINUS:
        checkNumberOperand(*(expr->op), *right);
        return make_shared<const Literal>(right->type, -boost::any_cast<double>(right->value));
    default:
        return NULL;
    }
}

boost::any Interpreter::visitVariableExpression(shared_ptr<const Variable> expr) const {}

void Interpreter::interpret(std::shared_ptr<Expression> expr)
{
    try
    {
        shared_ptr<const Literal> value =
            boost::any_cast<shared_ptr<const Literal>>(evaluate(expr));
        cout << stringify(*value) << endl;
    }
    catch (RuntimeError &error)
    {
        REPL::runtimeError(error);
    }
}
