#include <environment/environment.hpp>
#include <interpreter/interpreter.hpp>

using namespace Lox;
using namespace std;

Environment::Environment()
{
}

void Environment::define(const string name, const boost::any value)
{
    values[name] = value;
}

boost::any Environment::get(const Token &name)
{
    auto search = values.find(name.lexeme);

    if (search != values.end())
        return search->second;

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}