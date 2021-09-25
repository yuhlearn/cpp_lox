#include <iostream>
#include <repl/repl.hpp>

#include <scanner/token.hpp>
#include <ast/expression.hpp>
#include <ast/ast_printer.hpp>
#include <string>
#include <boost/any.hpp>

using namespace Lox;
using namespace std;
using namespace boost;

/*
int main(int, char **)
{
	//auto *exp = new Assign(new Token(TokenType::IDENTIFIER, "a", NULL, 1), new Literal(new Token(TokenType::NUMBER, "123", NULL, 1)));
	Expression *exp =
		new Binary(
			new Unary(
				new Token(TokenType::MINUS, "-", NULL, 1),
				new Literal(new Token(TokenType::NUMBER, "123", NULL, 1))),
			new Token(TokenType::STAR, "*", NULL, 1),
			new Grouping(
				new Literal(new Token(TokenType::NUMBER, "45.67", NULL, 1))));

	AstPrinter *visitor = new AstPrinter();

	cout << any_cast<string>(exp->accept(visitor)) << endl;

	delete exp;

	return 0;
}
*/

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		cout << "Usage: jlox [script]" << endl;
		return 1;
	}
	else if (argc == 2)
	{
		REPL::runFile(argv[1]);
	}
	else
	{
		REPL::runPrompt();
	}

	return 0;
}
