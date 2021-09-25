#ifndef _REPL_HPP
#define _REPL_HPP

#include <scanner/token.hpp>
#include <string>

namespace Lox
{
    class REPL
    {
    private:
        static bool hadError;
        REPL(void){};
        static void report(int line, std::string where, std::string message);
        static std::istream &getline(std::istream &__is, std::string &__str);

    public:
        static void error(int line, std::string message);
        static void error(Token token, std::string message);
        static void run(std::string source);
        static void runFile(char *path);
        static void runPrompt(void);
    };
}

#endif