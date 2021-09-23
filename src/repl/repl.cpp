#include <repl/repl.hpp>
#include <scanner/scanner.hpp>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>

using namespace Lox;
using namespace std;

bool REPL::hadError = false;

void REPL::error(int line, std::string message)
{
    report(line, "", message);
}

void REPL::report(int line, std::string where, std::string message)
{
    cerr << "[line " << line << "] Error" << where << ": " << message << endl;
    hadError = true;
}

void REPL::run(string source)
{
    Scanner scanner = Scanner(source);
    const list<Token> &tokens = scanner.scanTokens();

    for (const Token &token : tokens)
    {
        cout << token.toString() << endl;
    }
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