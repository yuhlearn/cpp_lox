#include <scanner/token.hpp>
#include <sstream>

using namespace Lox;
using namespace std;
using namespace boost;

Token::Token(TokenType type, string lexeme, any literal, int line)
    : type(type), lexeme(lexeme), literal(literal), line(line)
{
}

std::string Token::typeToString() const
{
    switch (type)
    {
    case TokenType::LEFT_PAREN:
        return "LEFT_PAREN";
        break;
    case TokenType::RIGHT_PAREN:
        return "RIGHT_PAREN";
        break;
    case TokenType::LEFT_BRACE:
        return "LEFT_BRACE";
        break;
    case TokenType::RIGHT_BRACE:
        return "RIGHT_BRACE";
        break;
    case TokenType::COMMA:
        return "COMMA";
        break;
    case TokenType::DOT:
        return "DOT";
        break;
    case TokenType::MINUS:
        return "MINUS";
        break;
    case TokenType::PLUS:
        return "PLUS";
        break;
    case TokenType::SEMICOLON:
        return "SEMICOLON";
        break;
    case TokenType::SLASH:
        return "SLASH";
        break;
    case TokenType::STAR:
        return "STAR";
        break;
    case TokenType::BANG:
        return "BANG";
        break;
    case TokenType::BANG_EQUAL:
        return "BANG_EQUAL";
        break;
    case TokenType::EQUAL:
        return "EQUAL";
        break;
    case TokenType::EQUAL_EQUAL:
        return "EQUAL_EQUAL";
        break;
    case TokenType::GREATER:
        return "GREATER";
        break;
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
        break;
    case TokenType::LESS:
        return "LESS";
        break;
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
        break;
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
        break;
    case TokenType::STRING:
        return "STRING";
        break;
    case TokenType::NUMBER:
        return "NUMBER";
        break;
    case TokenType::AND:
        return "AND";
        break;
    case TokenType::CLASS:
        return "CLASS";
        break;
    case TokenType::ELSE:
        return "ELSE";
        break;
    case TokenType::FALSE:
        return "FALSE";
        break;
    case TokenType::FUN:
        return "FUN";
        break;
    case TokenType::FOR:
        return "FOR";
        break;
    case TokenType::IF:
        return "IF";
        break;
    case TokenType::NIL:
        return "NIL";
        break;
    case TokenType::OR:
        return "OR";
        break;
    case TokenType::PRINT:
        return "PRINT";
        break;
    case TokenType::RETURN:
        return "RETURN";
        break;
    case TokenType::SUPER:
        return "SUPER";
        break;
    case TokenType::THIS:
        return "THIS";
        break;
    case TokenType::TRUE:
        return "TRUE";
        break;
    case TokenType::VAR:
        return "VAR";
        break;
    case TokenType::WHILE:
        return "WHILE";
        break;
    case TokenType::ENDOF:
        return "ENDOF";
        break;
    }
}

string Token::toString(void) const
{
    stringstream ss;

    //ss << "{" << (int)type << ", " << lexeme << ", " << literal.type().name() << ", " << line << "}";
    ss << lexeme;
    return ss.str();
}