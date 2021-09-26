#include <ast/ast_printer.hpp>
#include <ast/expression.hpp>

using namespace Lox;
using namespace std;

boost::any AstPrinter::visitAssignExpression(shared_ptr<const Assign> expr) const
{
    return "(Assign " + expr->name->toString() + " " +
           boost::any_cast<string>(expr->value->accept(*this)) + ")";
}

boost::any AstPrinter::visitBinaryExpression(shared_ptr<const Binary> expr) const
{
    return "(Binary " + expr->op->toString() + " " +
           boost::any_cast<string>(expr->left->accept(*this)) + " " +
           boost::any_cast<string>(expr->right->accept(*this)) + ")";
}

boost::any AstPrinter::visitCallExpression(shared_ptr<const Call> expr) const
{
    string str = "(Call " + boost::any_cast<string>(expr->callee->accept(*this));

    for (const auto subExpr : expr->arguments)
        str += " " + boost::any_cast<string>(subExpr->accept(*this));

    return str + ")";
}

boost::any AstPrinter::visitGetExpression(shared_ptr<const Get> expr) const
{
    return "(Get " + boost::any_cast<string>(expr->obj->accept(*this)) + " " +
           expr->name->toString() + ")";
}

boost::any AstPrinter::visitGroupingExpression(shared_ptr<const Grouping> expr) const
{
    return "(Group " + boost::any_cast<string>(expr->expression->accept(*this)) + ")";
}

boost::any AstPrinter::visitLiteralExpression(shared_ptr<const Literal> expr) const
{
    boost::any value = expr->value;

    switch (expr->type)
    {
    case TokenType::BOOLEAN:
        return boost::any_cast<bool>(value)
                   ? (string) "true"
                   : (string) "false";
    case TokenType::NIL:
        return (string) "nil";
    case TokenType::NUMBER:
        return std::to_string(boost::any_cast<double>(value));
    case TokenType::STRING:
        return "\"" + boost::any_cast<string>(value) + "\"";
    default:
        return "?";
    }
}

boost::any AstPrinter::visitLogicalExpression(shared_ptr<const Logical> expr) const
{
    return "(Logical " + expr->op->toString() + " " +
           boost::any_cast<string>(expr->left->accept(*this)) + " " +
           boost::any_cast<string>(expr->right->accept(*this)) + ")";
}

boost::any AstPrinter::visitSetExpression(shared_ptr<const Set> expr) const
{
    return "(Set " + boost::any_cast<string>(expr->obj->accept(*this)) + " " +
           expr->name->toString() + " " +
           boost::any_cast<string>(expr->value->accept(*this)) + ")";
}

boost::any AstPrinter::visitSuperExpression(shared_ptr<const Super> expr) const
{
    return "(Super " + expr->method->toString() + ")";
}

boost::any AstPrinter::visitThisExpression(shared_ptr<const This>) const
{
    return "This";
}

boost::any AstPrinter::visitUnaryExpression(shared_ptr<const Unary> expr) const
{
    return "(Unary " + expr->op->toString() + " " +
           boost::any_cast<string>(expr->right->accept(*this)) + ")";
}

boost::any AstPrinter::visitVariableExpression(shared_ptr<const Variable> expr) const
{
    return expr->name->toString();
}