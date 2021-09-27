#include <repl/repl.hpp>
#include <scanner/scanner.hpp>
#include <parser/parser.hpp>
#include <ast/ast_printer.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <boost/any.hpp>

using namespace Lox;
using namespace std;

bool REPL::hadError = false;
bool REPL::hadRuntimeError = false;
Interpreter REPL::interpreter = Interpreter();

void REPL::error(int line, std::string message)
{
    report(line, "", message);
}

void REPL::error(Token token, std::string message)
{
    if (token.type == TokenType::ENDOF)
        report(token.line, " at end ", message);
    else
        report(token.line, " at '" + token.lexeme + "'", message);
}

void REPL::runtimeError(RuntimeError error)
{
    cout << "[line " << error.token.line << "] " << error.message << endl;
    hadRuntimeError = true;
}

void REPL::report(int line, std::string where, std::string message)
{
    cerr << "[line " << line << "] Error" << where << ": " << message << endl;
    hadError = true;
}

void REPL::run(string source)
{
    Scanner scanner = Scanner(source);
    const vector<Token> &tokens = scanner.scanTokens();
    Parser parser = Parser(tokens);
    vector<shared_ptr<const Statement>> statements = parser.parse();

    if (hadError)
        return;

    interpreter.interpret(statements);
}

void REPL::runFile(char *path)
{
    ifstream in_file(path);
    string in_string;

    if (in_file.is_open())
    {
        in_string.assign(istreambuf_iterator<char>(in_file), istreambuf_iterator<char>());
    }
    else
    {
        cout << "Could not open " << path << endl;
    }

    in_file.close();

    run(in_string);

    if (hadError)
        exit(EXIT_FAILURE);

    if (hadRuntimeError)
        exit(EXIT_FAILURE);
}

istream &REPL::getline(istream &__is, string &__str)
{
    cout << "> ";
    return std::getline(__is, __str);
}

void REPL::runPrompt(void)
{
    for (string line; getline(cin, line);)
    {
        run(line);
        hadError = false;
    }
}