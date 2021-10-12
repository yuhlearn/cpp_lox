#ifndef _PRIMITIVES_HPP
#define _PRIMITIVES_HPP

#include <ast/callable.hpp>
#include <ast/value.hpp>
#include <interpreter/interpreter.hpp>
#include <memory>
#include <list>
#include <any>
#include <chrono>
#include <sys/time.h>
#include <ctime>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace Lox
{
    class LoxPrimitiveFn
    {
    public:
        static const Value clock(const std::list<Value> &)
        {
            double now = (double)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return Value(ValueType::NUMBER, (double)(now / 1000.0f));
        };
    };

    class LoxPrimitive : public LoxCallable
    {
    private:
        const Value (*func)(const std::list<Value> &);

    public:
        LoxPrimitive(void) = delete;

        LoxPrimitive(const Value (*func)(const std::list<Value> &)) : func(func)
        {
        }

        virtual long unsigned int arity(void) const override
        {
            return 0;
        }

        virtual Value call(const Interpreter &, const std::list<Value> &args) override
        {
            return func(args);
        }

        std::string toString(void) const
        {
            return "<native fn>";
        }
    };
}

#endif