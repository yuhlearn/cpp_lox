#include <scanner/scanner.hpp>
#include <repl/repl.hpp>
#include <iostream>

using namespace Lox;
using namespace std;

Scanner::Scanner(string source)
    : source(source), tokens(vector<Token>()), line(1), start(0), current(0)
{
}

bool Scanner::isAtEnd(void)
{
    return current >= (int)source.length();
}

bool Scanner::isAlpha(char c)
{
    return isalpha(c) || '_' == c;
}

bool Scanner::isAlphaNumeric(char c)
{
    return isAlpha(c) || isdigit(c);
}

char Scanner::advance(void)
{
    return source[current++];
}

void Scanner::addToken(TokenType type, std::any literal = nullptr)
{
    string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line));
}

bool Scanner::match(char expected)
{
    if (isAtEnd() || (source[current] != expected))
        return false;

    current++;
    return true;
}

char Scanner::peek(void)
{
    if (isAtEnd())
        return '\0';

    return source[current];
}

char Scanner::peekNext(void)
{
    if (current + 1 >= (int)source.length())
        return '\0';

    return source[current + 1];
}

void Scanner::matchString(void)
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
            line++;
        advance();
    }

    if (isAtEnd())
    {
        REPL::error(line, "Unterminated string.");
        return;
    }

    advance();

    string value = source.substr(start + 1, current - start - 2);

    addToken(TokenType::STRING, value);
}

void Scanner::matchNumber(void)
{
    while (isdigit(peek()))
        advance();

    if (peek() == '.' && isdigit(peekNext()))
    {
        advance();
        while (isdigit(peek()))
            advance();
    }

    addToken(TokenType::NUMBER, stod(source.substr(start, current - start)));
}

void Scanner::matchIdentifier(void)
{
    TokenType type = TokenType::IDENTIFIER;
    string literal;

    while (isAlphaNumeric(peek()))
        advance();

    literal = source.substr(start, current - start);

    auto search = keywords.find(literal);

    if (search != keywords.end())
        type = search->second;

    if (type != TokenType::BOOLEAN)
        addToken(type, literal);
    else
        addToken(type, "true" == literal);
}

void Scanner::scanToken(void)
{
    char c = advance();

    switch (c)
    {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case '.':
        addToken(TokenType::DOT);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;
    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '/':
        if (match('/'))
        {
            // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd())
                advance();
        }
        else
        {
            addToken(TokenType::SLASH);
        }
        break;
    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        break;
    case '\n':
        line++;
        break;
    case '"':
        matchString();
        break;
    default:
        if (isdigit(c))
            matchNumber();
        else if (isAlpha(c))
            matchIdentifier();
        else
            REPL::error(line, "Unexpected character.");
    }
}

const vector<Token> &Scanner::scanTokens(void)
{
    while (!isAtEnd())
    {
        start = current;
        scanToken();
    }

    tokens.push_back(Token(TokenType::ENDOF, "", "", line));

    return tokens;
}