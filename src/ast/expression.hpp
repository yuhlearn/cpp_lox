#ifndef _EXPRESSION_HPP
#define _EXPRESSION_HPP

#include <environment/environment.hpp>
#include <scanner/token.hpp>
#include <memory>
#include <utility>
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
		virtual boost::any visitAssignExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Assign> expr) const = 0;
		virtual boost::any visitBinaryExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Binary> expr) const = 0;
		virtual boost::any visitCallExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Call> expr) const = 0;
		virtual boost::any visitGetExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Get> expr) const = 0;
		virtual boost::any visitGroupingExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Grouping> expr) const = 0;
		virtual boost::any visitLiteralExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Literal> expr) const = 0;
		virtual boost::any visitLogicalExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Logical> expr) const = 0;
		virtual boost::any visitSetExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Set> expr) const = 0;
		virtual boost::any visitSuperExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Super> expr) const = 0;
		virtual boost::any visitThisExpression(std::shared_ptr<Environment> env, std::shared_ptr<const This> expr) const = 0;
		virtual boost::any visitUnaryExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Unary> expr) const = 0;
		virtual boost::any visitVariableExpression(std::shared_ptr<Environment> env, std::shared_ptr<const Variable> expr) const = 0;
	};

	class Expression
	{
	public:
		virtual ~Expression(void){};
		Expression(void){};
		virtual boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const = 0;
	};

	class Assign : public Expression, public std::enable_shared_from_this<Assign>
	{
	public:
		std::shared_ptr<const Token> name;
		std::shared_ptr<const Expression> value;

		Assign(std::shared_ptr<const Token> name, std::shared_ptr<const Expression> value)
			: name(name), value(value){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitAssignExpression(env, shared_from_this());
		}
	};

	class Binary : public Expression, public std::enable_shared_from_this<Binary>
	{
	public:
		std::shared_ptr<const Expression> left;
		std::shared_ptr<const Token> op;
		std::shared_ptr<const Expression> right;

		Binary(std::shared_ptr<const Expression> left, std::shared_ptr<const Token> op, std::shared_ptr<const Expression> right)
			: left(left), op(op), right(right){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitBinaryExpression(env, shared_from_this());
		}
	};

	class Call : public Expression, public std::enable_shared_from_this<Call>
	{
	public:
		std::shared_ptr<const Expression> callee;
		std::shared_ptr<const Token> paren;
		std::shared_ptr<std::list<std::shared_ptr<Expression>>> arguments;

		Call(std::shared_ptr<const Expression> callee, std::shared_ptr<const Token> paren, std::shared_ptr<std::list<std::shared_ptr<Expression>>> arguments)
			: callee(callee), paren(paren), arguments(arguments){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitCallExpression(env, shared_from_this());
		}
	};

	class Get : public Expression, public std::enable_shared_from_this<Get>
	{
	public:
		std::shared_ptr<const Expression> obj;
		std::shared_ptr<const Token> name;

		Get(std::shared_ptr<const Expression> obj, std::shared_ptr<const Token> name)
			: obj(obj), name(name){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitGetExpression(env, shared_from_this());
		}
	};

	class Grouping : public Expression, public std::enable_shared_from_this<Grouping>
	{
	public:
		std::shared_ptr<const Expression> expression;

		Grouping(std::shared_ptr<const Expression> expression)
			: expression(expression){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitGroupingExpression(env, shared_from_this());
		}
	};

	class Literal : public Expression, public std::enable_shared_from_this<Literal>
	{
	public:
		std::shared_ptr<const TokenType> type;
		std::shared_ptr<const boost::any> value;

		Literal(std::shared_ptr<const TokenType> type, std::shared_ptr<const boost::any> value)
			: type(type), value(value){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitLiteralExpression(env, shared_from_this());
		}
	};

	class Logical : public Expression, public std::enable_shared_from_this<Logical>
	{
	public:
		std::shared_ptr<const Expression> left;
		std::shared_ptr<const Token> op;
		std::shared_ptr<const Expression> right;

		Logical(std::shared_ptr<const Expression> left, std::shared_ptr<const Token> op, std::shared_ptr<const Expression> right)
			: left(left), op(op), right(right){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitLogicalExpression(env, shared_from_this());
		}
	};

	class Set : public Expression, public std::enable_shared_from_this<Set>
	{
	public:
		std::shared_ptr<const Expression> obj;
		std::shared_ptr<const Token> name;
		std::shared_ptr<const Expression> value;

		Set(std::shared_ptr<const Expression> obj, std::shared_ptr<const Token> name, std::shared_ptr<const Expression> value)
			: obj(obj), name(name), value(value){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitSetExpression(env, shared_from_this());
		}
	};

	class Super : public Expression, public std::enable_shared_from_this<Super>
	{
	public:
		std::shared_ptr<const Token> keyword;
		std::shared_ptr<const Token> method;

		Super(std::shared_ptr<const Token> keyword, std::shared_ptr<const Token> method)
			: keyword(keyword), method(method){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitSuperExpression(env, shared_from_this());
		}
	};

	class This : public Expression, public std::enable_shared_from_this<This>
	{
	public:
		std::shared_ptr<const Token> keyword;

		This(std::shared_ptr<const Token> keyword)
			: keyword(keyword){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitThisExpression(env, shared_from_this());
		}
	};

	class Unary : public Expression, public std::enable_shared_from_this<Unary>
	{
	public:
		std::shared_ptr<const Token> op;
		std::shared_ptr<const Expression> right;

		Unary(std::shared_ptr<const Token> op, std::shared_ptr<const Expression> right)
			: op(op), right(right){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitUnaryExpression(env, shared_from_this());
		}
	};

	class Variable : public Expression, public std::enable_shared_from_this<Variable>
	{
	public:
		std::shared_ptr<const Token> name;

		Variable(std::shared_ptr<const Token> name)
			: name(name){};

		boost::any accept(std::shared_ptr<Environment> env, const ExpressionVisitor &visitor) const override
		{
			return visitor.visitVariableExpression(env, shared_from_this());
		}
	};

};
#endif