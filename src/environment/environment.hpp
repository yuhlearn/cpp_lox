#ifndef _ENVIRONMENT_HPP
#define _ENVIRONMENT_HPP

#include <scanner/token.hpp>
#include <unordered_map>
#include <string>
#include <boost/any.hpp>

namespace Lox
{
    class Environment
    {
    private:
        std::unordered_map<std::string, boost::any> values;
        Environment *enclosing;

    public:
        Environment(void);
        Environment(Environment *enclosing);
        void define(const std::string &name, const boost::any &value);
        void assign(const Token &name, const boost::any &value);
        boost::any get(const Token &name);
    };
}

#endif