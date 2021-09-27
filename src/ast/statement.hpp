#ifndef _STATEMENT_HPP
#define _STATEMENT_HPP

#include <scanner/token.hpp>
#include <ast/expression.hpp>
#include <memory>
#include <utility>
#include <boost/any.hpp>

namespace Lox
{
	class Block;
	class Class;
	class Expression;
	class Function;
	class If;
	class Print;
	class Return;
	class Var;
	class While;

	class StatementVisitor
	{
	public:
		virtual ~StatementVisitor(void) {}
		virtual boost::any visitBlockStatement(std::shared_ptr<const Block> expr) const = 0;
		virtual boost::any visitClassStatement(std::shared_ptr<const Class> expr) const = 0;
		virtual boost::any visitExpressionStatement(std::shared_ptr<const Expression> expr) const = 0;
		virtual boost::any visitFunctionStatement(std::shared_ptr<const Function> expr) const = 0;
		virtual boost::any visitIfStatement(std::shared_ptr<const If> expr) const = 0;
		virtual boost::any visitPrintStatement(std::shared_ptr<const Print> expr) const = 0;
		virtual boost::any visitReturnStatement(std::shared_ptr<const Return> expr) const = 0;
		virtual boost::any visitVarStatement(std::shared_ptr<const Var> expr) const = 0;
		virtual boost::any visitWhileStatement(std::shared_ptr<const While> expr) const = 0;
	};

	class Statement
	{
	public:
		virtual ~Statement(void){};
		virtual boost::any accept(const StatementVisitor &visitor) const = 0;
	};

	class Block : public Statement, public std::enable_shared_from_this<Block>
	{
	public:
		std::list<std::shared_ptr<Statement>> statements;

		Block(std::list<std::shared_ptr<Statement>> statements)
			: statements(statements){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitBlockStatement(shared_from_this());
		}
	};

	class Class : public Statement, public std::enable_shared_from_this<Class>
	{
	public:
		std::shared_ptr<const Token> name;
		std::shared_ptr<const Variable> superclass;
		std::list<std::shared_ptr<Function>> methods;

		Class(std::shared_ptr<const Token> name, std::shared_ptr<const Variable> superclass, std::list<std::shared_ptr<Function>> methods)
			: name(name), superclass(superclass), methods(methods){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitClassStatement(shared_from_this());
		}
	};

	class Expression : public Statement, public std::enable_shared_from_this<Expression>
	{
	public:
		std::shared_ptr<const Expression> expression;

		Expression(std::shared_ptr<const Expression> expression)
			: expression(expression){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitExpressionStatement(shared_from_this());
		}
	};

	class Function : public Statement, public std::enable_shared_from_this<Function>
	{
	public:
		std::shared_ptr<const Token> name;
		std::list<std::shared_ptr<Token>> params;
		std::list<std::shared_ptr<Statement>> body;

		Function(std::shared_ptr<const Token> name, std::list<std::shared_ptr<Token>> params, std::list<std::shared_ptr<Statement>> body)
			: name(name), params(params), body(body){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitFunctionStatement(shared_from_this());
		}
	};

	class If : public Statement, public std::enable_shared_from_this<If>
	{
	public:
		std::shared_ptr<const Expression> condition;
		std::shared_ptr<const Statement> thenBranch;
		std::shared_ptr<const Statement> elseBranch;

		If(std::shared_ptr<const Expression> condition, std::shared_ptr<const Statement> thenBranch, std::shared_ptr<const Statement> elseBranch)
			: condition(condition), thenBranch(thenBranch), elseBranch(elseBranch){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitIfStatement(shared_from_this());
		}
	};

	class Print : public Statement, public std::enable_shared_from_this<Print>
	{
	public:
		std::shared_ptr<const Expression> expression;

		Print(std::shared_ptr<const Expression> expression)
			: expression(expression){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitPrintStatement(shared_from_this());
		}
	};

	class Return : public Statement, public std::enable_shared_from_this<Return>
	{
	public:
		std::shared_ptr<const Token> keyword;
		std::shared_ptr<const Expression> value;

		Return(std::shared_ptr<const Token> keyword, std::shared_ptr<const Expression> value)
			: keyword(keyword), value(value){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitReturnStatement(shared_from_this());
		}
	};

	class Var : public Statement, public std::enable_shared_from_this<Var>
	{
	public:
		std::shared_ptr<const Token> name;
		std::shared_ptr<const Expression> initializer;

		Var(std::shared_ptr<const Token> name, std::shared_ptr<const Expression> initializer)
			: name(name), initializer(initializer){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitVarStatement(shared_from_this());
		}
	};

	class While : public Statement, public std::enable_shared_from_this<While>
	{
	public:
		std::shared_ptr<const Expression> condition;
		std::shared_ptr<const Statement> body;

		While(std::shared_ptr<const Expression> condition, std::shared_ptr<const Statement> body)
			: condition(condition), body(body){};

		boost::any accept(const StatementVisitor &visitor) const override
		{
			return visitor.visitWhileStatement(shared_from_this());
		}
	};

};
#endif