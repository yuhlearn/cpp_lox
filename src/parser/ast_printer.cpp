#include <parser/ast_printer.hpp>
#include <parser/expression.hpp>

using namespace Lox;
using namespace boost;
using namespace std;

any AstPrinter::visitAssignExpression(const Assign *expr) const
{
    return "(Assign " + expr->name->toString() + " " +
           any_cast<string>(expr->value->accept(this)) + ")";
}

any AstPrinter::visitBinaryExpression(const Binary *expr) const
{
    return "(Binary " + expr->op->toString() + " " +
           any_cast<string>(expr->left->accept(this)) + " " +
           any_cast<string>(expr->right->accept(this)) + ")";
}

any AstPrinter::visitCallExpression(const Call *expr) const
{
    string str = "(Call " + any_cast<string>(expr->callee->accept(this));

    for (const Expression *subExpr : *expr->arguments)
        str += " " + any_cast<string>(subExpr->accept(this));

    return str + ")";
}

any AstPrinter::visitGetExpression(const Get *expr) const
{
    return "(Get " + any_cast<string>(expr->obj->accept(this)) + " " +
           expr->name->toString() + ")";
}

any AstPrinter::visitGroupingExpression(const Grouping *expr) const
{
    return "(Group " + any_cast<string>(expr->expression->accept(this)) + ")";
}

any AstPrinter::visitLiteralExpression(const Literal *expr) const
{
    return expr->value->toString();
}

any AstPrinter::visitLogicalExpression(const Logical *expr) const
{
    return "(Logical " + expr->op->toString() + " " +
           any_cast<string>(expr->left->accept(this)) + " " +
           any_cast<string>(expr->right->accept(this)) + ")";
}

any AstPrinter::visitSetExpression(const Set *expr) const
{
    return "(Set " + any_cast<string>(expr->obj->accept(this)) + " " +
           expr->name->toString() + " " +
           any_cast<string>(expr->value->accept(this)) + ")";
}

any AstPrinter::visitSuperExpression(const Super *expr) const
{
    return "(Super " + expr->method->toString() + ")";
}

any AstPrinter::visitThisExpression(const This *expr) const
{
    return "This";
}

any AstPrinter::visitUnaryExpression(const Unary *expr) const
{
    return "(Unary " + expr->op->toString() + " " +
           any_cast<string>(expr->right->accept(this)) + ")";
}

any AstPrinter::visitVariableExpression(const Variable *expr) const
{
    return expr->name->toString();
}