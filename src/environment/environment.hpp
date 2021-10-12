#ifndef _ENVIRONMENT_HPP
#define _ENVIRONMENT_HPP

#include <scanner/token.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <any>

namespace Lox
{
    class Environment
    {
    private:
        std::unordered_map<std::string, std::any> values;
        std::shared_ptr<Environment> enclosing;

        Environment *ancestor(const int distance);

    public:
        Environment(void);
        Environment(std::shared_ptr<Environment> enclosing);
        void define(const std::string &name, const std::any &value);
        void assign(const Token &name, const std::any &value);
        void assignAt(const int distance, const Token &name, const std::any &value);
        std::any get(const Token &name);
        std::any getAt(const int distance, const Token &name);
    };
}

#endif