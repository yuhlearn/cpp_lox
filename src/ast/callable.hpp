#ifndef _CALLABLE_H
#define _CALLABLE_H

#include <ast/value.hpp>
#include <interpreter/interpreter.hpp>
#include <memory>
#include <list>
#include <any>
#include <iostream>

namespace Lox
{
    class LoxCallable
    {
    public:
        virtual ~LoxCallable(void){};
        virtual int arity(void) const = 0;
        virtual Value call(const Interpreter &interpreter, const std::list<Value> &arguments) = 0;
        virtual std::string toString(void) const = 0;
    };
}

#endif