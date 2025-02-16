#include <resolver/resolver.hpp>
#include <repl/repl.hpp>

using namespace Lox;
using namespace std;

Resolver::Resolver(Interpreter &interpreter)
    : interpreter(interpreter),
      scopes(make_shared<deque<unordered_map<string, bool>>>()),
      currentFunction(new FunctionType())
{
    *currentFunction = FunctionType::NONE;
}

void Resolver::define(shared_ptr<const Token> name) const
{
    if (scopes->empty())
        return;

    scopes->back()[name->lexeme] = true;
}

void Resolver::declare(shared_ptr<const Token> name) const
{
    if (scopes->empty())
        return;

    unordered_map<string, bool> &scope = scopes->back();

    if (scope.find(name->lexeme) != scope.end())
        REPL::error(*name, "Already a variable with this name in this scope.");

    scope[name->lexeme] = false;
}

void Resolver::beginScope(void) const
{
    scopes->push_back(unordered_map<string, bool>());
}

void Resolver::endScope(void) const
{
    scopes->pop_back();
}

void Resolver::resolve(shared_ptr<Environment> env, shared_ptr<const Expression> expr) const
{
    expr->accept(env, *this);
}

void Resolver::resolve(shared_ptr<Environment> env, shared_ptr<const Statement> stmt) const
{
    stmt->accept(env, *this);
}

void Resolver::resolve(shared_ptr<Environment> env, shared_ptr<list<shared_ptr<Statement>>> statements) const
{
    for (auto stmt : *statements)
        resolve(env, stmt);
}

void Resolver::resolve(shared_ptr<Environment> env, vector<shared_ptr<const Statement>> &statements) const
{
    for (auto stmt : statements)
        resolve(env, stmt);
}

void Resolver::resolveLocal(shared_ptr<Environment> env,
                            shared_ptr<const Expression> expr,
                            shared_ptr<const Token> name) const
{
    int depth = 0;
    auto scope = scopes->rbegin();

    while (scope != scopes->rend())
    {
        auto search = scope->find(name->lexeme);

        if (search != scope->end())
        {
            interpreter.resolve(env, expr, depth);
            return;
        }
        scope++;
        depth++;
    }
}

void Resolver::resolveFunction(shared_ptr<Environment> env,
                               shared_ptr<const Function> function,
                               const FunctionType type) const
{
    FunctionType enclosingFunction = *currentFunction;
    *currentFunction = type;

    beginScope();

    for (auto param : *function->params)
    {
        declare(param);
        define(param);
    }

    resolve(env, function->body);
    endScope();

    *currentFunction = enclosingFunction;
}

// EXPRESSIONS
std::any Resolver::visitAssignExpression(shared_ptr<Environment> env, shared_ptr<const Assign> expr) const
{
    resolve(env, expr->value);
    resolveLocal(env, expr, expr->name);
    return nullptr;
}

std::any Resolver::visitBinaryExpression(shared_ptr<Environment> env, shared_ptr<const Binary> expr) const
{
    resolve(env, expr->left);
    resolve(env, expr->right);
    return nullptr;
}

std::any Resolver::visitCallExpression(shared_ptr<Environment> env, shared_ptr<const Call> expr) const
{
    resolve(env, expr->callee);

    for (auto arg : *expr->arguments)
        resolve(env, arg);

    return nullptr;
}

std::any Resolver::visitGetExpression(shared_ptr<Environment>, shared_ptr<const Get>) const
{
    return nullptr;
}

std::any Resolver::visitGroupingExpression(shared_ptr<Environment> env, shared_ptr<const Grouping> expr) const
{
    resolve(env, expr->expression);
    return nullptr;
}

std::any Resolver::visitLiteralExpression(shared_ptr<Environment>, shared_ptr<const Literal>) const
{
    return nullptr;
}

std::any Resolver::visitLogicalExpression(shared_ptr<Environment> env, shared_ptr<const Logical> expr) const
{
    resolve(env, expr->left);
    resolve(env, expr->right);
    return nullptr;
}

std::any Resolver::visitSetExpression(shared_ptr<Environment>, shared_ptr<const Set>) const
{
    return nullptr;
}

std::any Resolver::visitSuperExpression(shared_ptr<Environment>, shared_ptr<const Super>) const
{
    return nullptr;
}

std::any Resolver::visitThisExpression(shared_ptr<Environment>, shared_ptr<const This>) const
{
    return nullptr;
}

std::any Resolver::visitUnaryExpression(shared_ptr<Environment> env, shared_ptr<const Unary> expr) const
{
    resolve(env, expr->right);
    return nullptr;
}

std::any Resolver::visitVariableExpression(shared_ptr<Environment> env, shared_ptr<const Variable> expr) const
{
    if (!scopes->empty())
    {
        auto search = scopes->back().find(expr->name->lexeme);
        if (search != scopes->back().end() && search->second == false)
            REPL::error(*(expr->name), "Can't read local variable in its own initializer.");
    }

    resolveLocal(env, expr, expr->name);

    return nullptr;
}

// STATEMENTS
std::any Resolver::visitBlockStatement(shared_ptr<Environment> env, shared_ptr<const Block> stmt) const
{
    beginScope();
    resolve(env, stmt->statements);
    endScope();
    return nullptr;
}

std::any Resolver::visitClassStatement(shared_ptr<Environment>, shared_ptr<const Class>) const
{
    return nullptr;
}

std::any Resolver::visitExpressionStatementStatement(shared_ptr<Environment> env,
                                                     shared_ptr<const ExpressionStatement> stmt) const
{
    resolve(env, stmt->expression);
    return nullptr;
}

std::any Resolver::visitFunctionStatement(shared_ptr<Environment> env, shared_ptr<const Function> stmt) const
{
    declare(stmt->name);
    define(stmt->name);

    resolveFunction(env, stmt, FunctionType::FUNCTION);
    return nullptr;
}

std::any Resolver::visitIfStatement(shared_ptr<Environment> env, shared_ptr<const If> stmt) const
{
    resolve(env, stmt->condition);
    resolve(env, stmt->thenBranch);

    if (stmt->elseBranch != nullptr)
        resolve(env, stmt->elseBranch);

    return nullptr;
}

std::any Resolver::visitPrintStatement(shared_ptr<Environment> env, shared_ptr<const Print> stmt) const
{
    resolve(env, stmt->expression);
    return nullptr;
}

std::any Resolver::visitReturnStatement(shared_ptr<Environment> env, shared_ptr<const Return> stmt) const
{
    if (*currentFunction == FunctionType::NONE)
        REPL::error(*(stmt->keyword), "Can't return from top-level code.");

    if (stmt->value != nullptr)
        resolve(env, stmt->value);

    return nullptr;
}

std::any Resolver::visitVarStatement(shared_ptr<Environment> env, shared_ptr<const Var> stmt) const
{
    declare(stmt->name);

    if (stmt->initializer != nullptr)
        resolve(env, stmt->initializer);

    define(stmt->name);
    return nullptr;
}

std::any Resolver::visitWhileStatement(shared_ptr<Environment> env, shared_ptr<const While> stmt) const
{
    resolve(env, stmt->condition);
    resolve(env, stmt->body);
    return nullptr;
}