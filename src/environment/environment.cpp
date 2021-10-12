#include <environment/environment.hpp>
#include <interpreter/interpreter.hpp>

using namespace Lox;
using namespace std;

Environment::Environment(void)
    : values(std::unordered_map<string, std::any>()), enclosing(nullptr)
{
}

Environment::Environment(shared_ptr<Environment> enclosing)
    : values(std::unordered_map<string, std::any>()), enclosing(enclosing)
{
}

void Environment::define(const string &name, const std::any &value)
{
    values[name] = value;
}

void Environment::assign(const Token &name, const std::any &value)
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

void Environment::assignAt(const int distance, const Token &name, const std::any &value)
{
    Environment *env = ancestor(distance);

    if (env->values.find(name.lexeme) != env->values.end())
    {
        env->values[name.lexeme] = value;
        return;
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::any Environment::get(const Token &name)
{
    auto search = values.find(name.lexeme);

    if (search != values.end())
        return search->second;

    if (enclosing != nullptr)
        return enclosing->get(name);

    throw RuntimeError(name, "Undefined global variable '" + name.lexeme + "'.");
}

std::any Environment::getAt(const int distance, const Token &name)
{
    Environment *env = ancestor(distance);

    auto search = env->values.find(name.lexeme);

    if (search != values.end())
        return search->second;

    throw RuntimeError(name, "Undefined local variable '" + name.lexeme + "'.");
}

Environment *Environment::ancestor(const int distance)
{
    Environment *env = this;

    for (int i = 0; i < distance; i++)
        env = &(*env->enclosing);

    return env;
}