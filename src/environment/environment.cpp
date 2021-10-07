#include <environment/environment.hpp>
#include <interpreter/interpreter.hpp>

using namespace Lox;
using namespace std;

Environment::Environment(void)
    : values(std::unordered_map<string, boost::any>()), enclosing(nullptr)
{
}

Environment::Environment(shared_ptr<Environment> enclosing)
    : values(std::unordered_map<string, boost::any>()), enclosing(enclosing)
{
}

void Environment::define(const string &name, const boost::any &value)
{
    values[name] = value;
}

void Environment::assign(const Token &name, const boost::any &value)
{
    if (values.find(name.lexeme) != values.end())
    {
        values[name.lexeme] = value;
        return;
    }

    if (enclosing != nullptr)
    {
        enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

boost::any Environment::get(const Token &name)
{
    auto search = values.find(name.lexeme);

    if (search != values.end())
        return search->second;

    if (enclosing != nullptr)
        return enclosing->get(name);

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}