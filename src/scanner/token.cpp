#include <scanner/token.hpp>
#include <sstream>

using namespace Lox;
using namespace std;
using namespace boost;

Token::Token(TokenType type, string lexeme, any literal, int line)
    : type(type), lexeme(lexeme), literal(literal), line(line)
{
}

string Token::toString(void) const
{
    stringstream ss;

    ss << "{" << (int)type << ", " << lexeme << ", " << literal.type().name() << ", " << line << "}";

    return ss.str();
}