#ifndef _STATEMENT_HPP
#define _STATEMENT_HPP

#include <environment/environment.hpp>
#include <scanner/token.hpp>
#include <ast/expression.hpp>
#include <memory>
#include <utility>
#include <any>

namespace Lox
{
	class Block;
	class Class;
	class ExpressionStatement;
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
		virtual std::any visitBlockStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Block> expr) const = 0;
		virtual std::any visitClassStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Class> expr) const = 0;
		virtual std::any visitExpressionStatementStatement(std::shared_ptr<Environment> env, std::shared_ptr<const ExpressionStatement> expr) const = 0;
		virtual std::any visitFunctionStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Function> expr) const = 0;
		virtual std::any visitIfStatement(std::shared_ptr<Environment> env, std::shared_ptr<const If> expr) const = 0;
		virtual std::any visitPrintStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Print> expr) const = 0;
		virtual std::any visitReturnStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Return> expr) const = 0;
		virtual std::any visitVarStatement(std::shared_ptr<Environment> env, std::shared_ptr<const Var> expr) const = 0;
		virtual std::any visitWhileStatement(std::shared_ptr<Environment> env, std::shared_ptr<const While> expr) const = 0;
	};

	class Statement
	{
	public:
		virtual ~Statement(void){};
		Statement(void){};
		virtual std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const = 0;
	};

	class Block : public Statement, public std::enable_shared_from_this<Block>
	{
	public:
		std::shared_ptr<std::list<std::shared_ptr<Statement>>> statements;

		Block(std::shared_ptr<std::list<std::shared_ptr<Statement>>> statements)
			: statements(statements){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitBlockStatement(env, shared_from_this());
		}
	};

	class Class : public Statement, public std::enable_shared_from_this<Class>
	{
	public:
		std::shared_ptr<const Token> name;
		std::shared_ptr<const Variable> superclass;
		std::shared_ptr<std::list<std::shared_ptr<Function>>> methods;

		Class(std::shared_ptr<const Token> name, std::shared_ptr<const Variable> superclass, std::shared_ptr<std::list<std::shared_ptr<Function>>> methods)
			: name(name), superclass(superclass), methods(methods){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitClassStatement(env, shared_from_this());
		}
	};

	class ExpressionStatement : public Statement, public std::enable_shared_from_this<ExpressionStatement>
	{
	public:
		std::shared_ptr<const Expression> expression;

		ExpressionStatement(std::shared_ptr<const Expression> expression)
			: expression(expression){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitExpressionStatementStatement(env, shared_from_this());
		}
	};

	class Function : public Statement, public std::enable_shared_from_this<Function>
	{
	public:
		std::shared_ptr<const Token> name;
		std::shared_ptr<std::list<std::shared_ptr<Token>>> params;
		std::shared_ptr<std::list<std::shared_ptr<Statement>>> body;

		Function(std::shared_ptr<const Token> name, std::shared_ptr<std::list<std::shared_ptr<Token>>> params, std::shared_ptr<std::list<std::shared_ptr<Statement>>> body)
			: name(name), params(params), body(body){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitFunctionStatement(env, shared_from_this());
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

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitIfStatement(env, shared_from_this());
		}
	};

	class Print : public Statement, public std::enable_shared_from_this<Print>
	{
	public:
		std::shared_ptr<const Expression> expression;

		Print(std::shared_ptr<const Expression> expression)
			: expression(expression){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitPrintStatement(env, shared_from_this());
		}
	};

	class Return : public Statement, public std::enable_shared_from_this<Return>
	{
	public:
		std::shared_ptr<const Token> keyword;
		std::shared_ptr<const Expression> value;

		Return(std::shared_ptr<const Token> keyword, std::shared_ptr<const Expression> value)
			: keyword(keyword), value(value){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitReturnStatement(env, shared_from_this());
		}
	};

	class Var : public Statement, public std::enable_shared_from_this<Var>
	{
	public:
		std::shared_ptr<const Token> name;
		std::shared_ptr<const Expression> initializer;

		Var(std::shared_ptr<const Token> name, std::shared_ptr<const Expression> initializer)
			: name(name), initializer(initializer){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitVarStatement(env, shared_from_this());
		}
	};

	class While : public Statement, public std::enable_shared_from_this<While>
	{
	public:
		std::shared_ptr<const Expression> condition;
		std::shared_ptr<const Statement> body;

		While(std::shared_ptr<const Expression> condition, std::shared_ptr<const Statement> body)
			: condition(condition), body(body){};

		std::any accept(std::shared_ptr<Environment> env, const StatementVisitor &visitor) const override
		{
			return visitor.visitWhileStatement(env, shared_from_this());
		}
	};

};
#endif