#include <iostream>
#include <repl/repl.hpp>

#include <scanner/token.hpp>
#include <parser/expression.hpp>
#include <parser/ast_printer.hpp>
#include <string>

using namespace std;
using namespace Lox;

int main(int argc, char **argv)
{
	auto *exp = new Assign(new Token(TokenType::MINUS, "-", NULL, 1), new Literal(new Token(TokenType::MINUS, "-", NULL, 1)));

	AstPrinter *visitor = new AstPrinter();

	exp->accept(visitor);

	return 0;
}

/* 
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
} */
