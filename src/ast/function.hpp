#ifndef _FUNCTION_HPP
#define _FUNCTION_HPP

#include <ast/callable.hpp>
#include <ast/value.hpp>
#include <ast/return_value.hpp>
#include <interpreter/interpreter.hpp>
#include <memory>
#include <list>
#include <any>

namespace Lox
{
    class LoxFunction : public LoxCallable
    {
    private:
        std::shared_ptr<const Function> declaration;
        std::shared_ptr<Environment> closure;

    public:
        LoxFunction(void) = delete;

        LoxFunction(std::shared_ptr<const Function> declaration, std::shared_ptr<Environment> closure)
            : declaration(declaration), closure(closure)
        {
        }

        virtual long unsigned int arity(void) const override
        {
            return declaration->params->size();
        }

        virtual Value call(const Interpreter &interpreter, const std::list<Value> &args) override
        {
            std::shared_ptr<Environment> env = std::make_shared<Environment>(closure);
            std::list<std::shared_ptr<Token>> &params = *declaration->params;
            auto param = params.begin();
            auto arg = args.begin();

            for (; param != params.end(); param++, arg++)
                env->define((*param)->lexeme, *arg);

            try
            {
                interpreter.executeBlock(env, declaration->body);
            }
            catch (ReturnValue &rv)
            {
                return rv.value;
            }
            return Value(ValueType::NIL, nullptr);
        }

        std::string toString(void) const
        {
            return "<fn " + declaration->name->lexeme + ">";
        }
    };
}

#endif