#ifndef _AST_PRINTER_HPP
#define _AST_PRINTER_HPP

#include <ast/expression.hpp>
#include <string>

namespace Lox
{
    class AstPrinter : public ExpressionVisitor
    {
    public:
        boost::any visitAssignExpression(std::shared_ptr<const Assign> expr) const;
        boost::any visitBinaryExpression(std::shared_ptr<const Binary> expr) const;
        boost::any visitCallExpression(std::shared_ptr<const Call> expr) const;
        boost::any visitGetExpression(std::shared_ptr<const Get> expr) const;
        boost::any visitGroupingExpression(std::shared_ptr<const Grouping> expr) const;
        boost::any visitLiteralExpression(std::shared_ptr<const Literal> expr) const;
        boost::any visitLogicalExpression(std::shared_ptr<const Logical> expr) const;
        boost::any visitSetExpression(std::shared_ptr<const Set> expr) const;
        boost::any visitSuperExpression(std::shared_ptr<const Super> expr) const;
        boost::any visitThisExpression(std::shared_ptr<const This> expr) const;
        boost::any visitUnaryExpression(std::shared_ptr<const Unary> expr) const;
        boost::any visitVariableExpression(std::shared_ptr<const Variable> expr) const;
    };
}

#endif