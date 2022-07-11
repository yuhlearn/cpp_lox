#include <iostream>
#include <repl/repl.hpp>

#include <scanner/token.hpp>
#include <ast/expression.hpp>
#include <string>
#include <any>

using namespace Lox;
using namespace std;

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		cout << "Usage: cpp_lox [script]" << endl;
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
