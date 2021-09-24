#ifndef _EXPRESSION_HPP
#define _EXPRESSION_HPP

#include <scanner/token.hpp>
#include <boost/any.hpp>

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

	class ExpressionVisitor
	{
	public:
		virtual ~ExpressionVisitor(void) {}
		virtual boost::any visitAssignExpression(const Assign *expr) const = 0;
		virtual boost::any visitBinaryExpression(const Binary *expr) const = 0;
		virtual boost::any visitCallExpression(const Call *expr) const = 0;
		virtual boost::any visitGetExpression(const Get *expr) const = 0;
		virtual boost::any visitGroupingExpression(const Grouping *expr) const = 0;
		virtual boost::any visitLiteralExpression(const Literal *expr) const = 0;
		virtual boost::any visitLogicalExpression(const Logical *expr) const = 0;
		virtual boost::any visitSetExpression(const Set *expr) const = 0;
		virtual boost::any visitSuperExpression(const Super *expr) const = 0;
		virtual boost::any visitThisExpression(const This *expr) const = 0;
		virtual boost::any visitUnaryExpression(const Unary *expr) const = 0;
		virtual boost::any visitVariableExpression(const Variable *expr) const = 0;
	};

	class Expression
	{
	public:
		virtual ~Expression(void){};
		virtual boost::any accept(const ExpressionVisitor *visitor) const = 0;
	};

	class Assign : public Expression
	{
	public:
		const Token *name;
		const Expression *value;

		Assign(const Token *name, const Expression *value)
			: name(name), value(value){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitAssignExpression(this);
		}
	};

	class Binary : public Expression
	{
	public:
		const Expression *left;
		const Token *op;
		const Expression *right;

		Binary(const Expression *left, const Token *op, const Expression *right)
			: left(left), op(op), right(right){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitBinaryExpression(this);
		}
	};

	class Call : public Expression
	{
	public:
		const Expression *callee;
		const Token *paren;
		const std::list<Expression *> *arguments;

		Call(const Expression *callee, const Token *paren, const std::list<Expression *> *arguments)
			: callee(callee), paren(paren), arguments(arguments){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitCallExpression(this);
		}
	};

	class Get : public Expression
	{
	public:
		const Expression *obj;
		const Token *name;

		Get(const Expression *obj, const Token *name)
			: obj(obj), name(name){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitGetExpression(this);
		}
	};

	class Grouping : public Expression
	{
	public:
		const Expression *expression;

		Grouping(const Expression *expression)
			: expression(expression){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitGroupingExpression(this);
		}
	};

	class Literal : public Expression
	{
	public:
		const Token *value;

		Literal(const Token *value)
			: value(value){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitLiteralExpression(this);
		}
	};

	class Logical : public Expression
	{
	public:
		const Expression *left;
		const Token *op;
		const Expression *right;

		Logical(const Expression *left, const Token *op, const Expression *right)
			: left(left), op(op), right(right){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitLogicalExpression(this);
		}
	};

	class Set : public Expression
	{
	public:
		const Expression *obj;
		const Token *name;
		const Expression *value;

		Set(const Expression *obj, const Token *name, const Expression *value)
			: obj(obj), name(name), value(value){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitSetExpression(this);
		}
	};

	class Super : public Expression
	{
	public:
		const Token *keyword;
		const Token *method;

		Super(const Token *keyword, const Token *method)
			: keyword(keyword), method(method){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitSuperExpression(this);
		}
	};

	class This : public Expression
	{
	public:
		const Token *keyword;

		This(const Token *keyword)
			: keyword(keyword){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitThisExpression(this);
		}
	};

	class Unary : public Expression
	{
	public:
		const Token *op;
		const Expression *right;

		Unary(const Token *op, const Expression *right)
			: op(op), right(right){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitUnaryExpression(this);
		}
	};

	class Variable : public Expression
	{
	public:
		const Token *name;

		Variable(const Token *name)
			: name(name){};

		boost::any accept(const ExpressionVisitor *visitor) const override
		{
			return visitor->visitVariableExpression(this);
		}
	};

};
#endif