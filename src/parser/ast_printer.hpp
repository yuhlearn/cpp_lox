#ifndef _AST_PRINTER_HPP
#define _AST_PRINTER_HPP

#include <parser/expression.hpp>
#include <string>

namespace Lox
{
    class AstPrinter : public Visitor
    {
    public:
        void visitAssignExpr(const Assign *expr) const override;
        // std::string visitBinaryExpr(Binary expr);
        // std::string visitCallExpr(Call expr);
        // std::string visitGetExpr(Get expr);
        // std::string visitGroupingExpr(Grouping expr);
        void visitLiteralExpr(const Literal *expr) const override;
        // std::string visitLogicalExpr(Logical expr);
        // std::string visitSetExpr(Set expr);
        // std::string visitSuperExpr(Super expr);
        // std::string visitThisExpr(This expr);
        // std::string visitUnaryExpr(Unary expr);
        // std::string visitVariableExpr(Variable expr);
    };
}

#endif