#ifndef _RETURN_VALUE_HPP
#define _RETURN_VALUE_HPP

#include <ast/value.hpp>
#include <exception>

namespace Lox
{
    class ReturnValue : public std::exception
    {
    public:
        const Value value;

        ReturnValue(Value value) : value(value)
        {
        }
    };
}

#endif