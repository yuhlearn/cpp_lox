#ifndef _EXPTESSION_HPP
#define _EXPTESSION_HPP

#include <scanner/token.hpp>

namespace Lox
{

    class Assign;
    class Binary;
    class Call;
    class Get;
    class Grouping;
    class Literal;
    class Logical;
    class Set;
    class Super;
    class This;
    class Unary;
    class Variable;

    class Visitor
    {
    public:
        virtual ~Visitor() {}
        virtual void visitAssignExpr(const Assign *expr) const = 0;
        // virtual T visitBinaryExpr(Binary expr) = 0;
        // virtual T visitCallExpr(Call expr) = 0;
        // virtual T visitGetExpr(Get expr) = 0;
        // virtual T visitGroupingExpr(Grouping expr) = 0;
        virtual void visitLiteralExpr(const Literal *expr) const = 0;
        // virtual T visitLogicalExpr(Logical expr) = 0;
        // virtual T visitSetExpr(Set expr) = 0;
        // virtual T visitSuperExpr(Super expr) = 0;
        // virtual T visitThisExpr(This expr) = 0;
        // virtual T visitUnaryExpr(Unary expr) = 0;
        // virtual T visitVariableExpr(Variable expr) = 0;
    };

    class Expression
    {
    public:
        virtual ~Expression(void){};
        virtual void accept(const Visitor *visitor) const = 0;
    };

    class Assign : public Expression
    {
    public:
        const Token *name;
        const Expression *value;

        Assign(Token *name, Expression *value)
            : name(name), value(value){};

        void accept(const Visitor *visitor) const override
        {
            return visitor->visitAssignExpr(this);
        };
    };

    class Literal : public Expression
    {
    public:
        const Token *value;

        Literal(Token *value)
            : value(value){};

        void accept(const Visitor *visitor) const override
        {
            return visitor->visitLiteralExpr(this);
        };
    };
}

#endif