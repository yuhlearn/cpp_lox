#ifndef _AST_PRINTER_HPP
#define _AST_PRINTER_HPP

#include <parser/expression.hpp>
#include <string>

namespace Lox
{
    class AstPrinter : public ExpressionVisitor
    {
    public:
        boost::any visitAssignExpression(const Assign *expr) const override;
        boost::any visitBinaryExpression(const Binary *expr) const override;
        boost::any visitCallExpression(const Call *expr) const override;
        boost::any visitGetExpression(const Get *expr) const override;
        boost::any visitGroupingExpression(const Grouping *expr) const override;
        boost::any visitLiteralExpression(const Literal *expr) const override;
        boost::any visitLogicalExpression(const Logical *expr) const override;
        boost::any visitSetExpression(const Set *expr) const override;
        boost::any visitSuperExpression(const Super *expr) const override;
        boost::any visitThisExpression(const This *expr) const override;
        boost::any visitUnaryExpression(const Unary *expr) const override;
        boost::any visitVariableExpression(const Variable *expr) const override;
    };
}

#endif