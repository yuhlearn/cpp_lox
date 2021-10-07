#ifndef _VALUE_HPP
#define _VALUE_HPP

#include <scanner/token.hpp>
#include <memory>
#include <utility>
#include <boost/any.hpp>

namespace Lox
{
    enum class ValueType
    {
        PRIMITIVE,
        FUNCTION,
        IDENTIFIER,
        STRING,
        NUMBER,
        BOOLEAN,
        NIL,
    };

    class Value
    {
    public:
        const ValueType type;
        const boost::any value;

        Value(const ValueType type, const boost::any value)
            : type(type), value(value){};
    };
};
#endif