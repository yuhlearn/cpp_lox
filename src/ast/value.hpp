#ifndef _VALUE_HPP
#define _VALUE_HPP

#include <scanner/token.hpp>
#include <memory>
#include <utility>
#include <boost/any.hpp>

namespace Lox
{
    class Value
    {
    public:
        const TokenType type;
        const boost::any value;

        Value(const TokenType type, const boost::any value)
            : type(type), value(value){};
    };
};
#endif