#ifndef _AST_PRINTER_HPP
#define _AST_PRINTER_HPP

#include <ast/expression.hpp>
#include <string>
#include <memory>
#include <boost/any.hpp>

namespace Lox
{
    class AstPrinter : public ExpressionVisitor
    {
    public:
        boost::any visitAssignExpression(std::shared_ptr<const Assign> expr) const override;
        boost::any visitBinaryExpression(std::shared_ptr<const Binary> expr) const override;
        boost::any visitCallExpression(std::shared_ptr<const Call> expr) const override;
        boost::any visitGetExpression(std::shared_ptr<const Get> expr) const override;
        boost::any visitGroupingExpression(std::shared_ptr<const Grouping> expr) const override;
        boost::any visitLiteralExpression(std::shared_ptr<const Literal> expr) const override;
        boost::any visitLogicalExpression(std::shared_ptr<const Logical> expr) const override;
        boost::any visitSetExpression(std::shared_ptr<const Set> expr) const override;
        boost::any visitSuperExpression(std::shared_ptr<const Super> expr) const override;
        boost::any visitThisExpression(std::shared_ptr<const This> expr) const override;
        boost::any visitUnaryExpression(std::shared_ptr<const Unary> expr) const override;
        boost::any visitVariableExpression(std::shared_ptr<const Variable> expr) const override;
    };
}

#endif