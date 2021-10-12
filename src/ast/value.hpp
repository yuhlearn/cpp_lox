#ifndef _VALUE_HPP
#define _VALUE_HPP

#include <scanner/token.hpp>
#include <memory>
#include <utility>
#include <any>

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
        const std::any value;

        Value(const ValueType type, const std::any value)
            : type(type), value(value){};
    };
};
#endif