#include <parser/ast_printer.hpp>
#include <parser/expression.hpp>
#include <iostream>

using namespace Lox;
using namespace std;

void AstPrinter::visitAssignExpr(const Assign *expr) const
{
    cout << "Assign" << endl;
    expr->value->accept(this);
}

void AstPrinter::visitLiteralExpr(const Literal *expr) const
{
    cout << "Literal" << endl;
}
