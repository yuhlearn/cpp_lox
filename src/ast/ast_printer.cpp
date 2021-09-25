#include <ast/ast_printer.hpp>
#include <ast/expression.hpp>

using namespace Lox;
using namespace boost;
using namespace std;

any AstPrinter::visitAssignExpression(std::shared_ptr<const Assign> expr) const
{
    return "(Assign " + expr->name->toString() + " " +
           any_cast<string>(expr->value->accept(*this)) + ")";
}

any AstPrinter::visitBinaryExpression(std::shared_ptr<const Binary> expr) const
{
    return "(Binary " + expr->op->toString() + " " +
           any_cast<string>(expr->left->accept(*this)) + " " +
           any_cast<string>(expr->right->accept(*this)) + ")";
}

any AstPrinter::visitCallExpression(std::shared_ptr<const Call> expr) const
{
    string str = "(Call " + any_cast<string>(expr->callee->accept(*this));

    for (const auto subExpr : expr->arguments)
        str += " " + any_cast<string>(subExpr->accept(*this));

    return str + ")";
}

any AstPrinter::visitGetExpression(std::shared_ptr<const Get> expr) const
{
    return "(Get " + any_cast<string>(expr->obj->accept(*this)) + " " +
           expr->name->toString() + ")";
}

any AstPrinter::visitGroupingExpression(std::shared_ptr<const Grouping> expr) const
{
    return "(Group " + any_cast<string>(expr->expression->accept(*this)) + ")";
}

any AstPrinter::visitLiteralExpression(std::shared_ptr<const Literal> expr) const
{
    return expr->value->toString();
}

any AstPrinter::visitLogicalExpression(std::shared_ptr<const Logical> expr) const
{
    return "(Logical " + expr->op->toString() + " " +
           any_cast<string>(expr->left->accept(*this)) + " " +
           any_cast<string>(expr->right->accept(*this)) + ")";
}

any AstPrinter::visitSetExpression(std::shared_ptr<const Set> expr) const
{
    return "(Set " + any_cast<string>(expr->obj->accept(*this)) + " " +
           expr->name->toString() + " " +
           any_cast<string>(expr->value->accept(*this)) + ")";
}

any AstPrinter::visitSuperExpression(std::shared_ptr<const Super> expr) const
{
    return "(Super " + expr->method->toString() + ")";
}

any AstPrinter::visitThisExpression(std::shared_ptr<const This>) const
{
    return "This";
}

any AstPrinter::visitUnaryExpression(std::shared_ptr<const Unary> expr) const
{
    return "(Unary " + expr->op->toString() + " " +
           any_cast<string>(expr->right->accept(*this)) + ")";
}

any AstPrinter::visitVariableExpression(std::shared_ptr<const Variable> expr) const
{
    return expr->name->toString();
}