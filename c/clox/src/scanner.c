#include "scanner.h"

#include "assert.h"
#include "common.h"

#include <string.h>

static bool isAtEnd (
    );
static char advance (
    );
static Token makeToken (
    TokenType type);
static Token errorToken (
    const char *message);

Scanner scanner;

void
initScanner (
    const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

Token
scanToken (
    )
{
    scanner.start = scanner.current;
    if (isAtEnd ())
        return makeToken (TOKEN_EOF);

    char c = advance ();

    switch (c) {
    case '(':
        return makeToken (TOKEN_LEFT_PAREN);
    case ')':
        return makeToken (TOKEN_RIGHT_PAREN);

    case '{':
        return makeToken (TOKEN_LEFT_BRACE);
    case '}':
        return makeToken (TOKEN_RIGHT_BRACE);

    case ',':
        return makeToken (TOKEN_COMMA);
    case '.':
        return makeToken (TOKEN_DOT);
    case '-':
        return makeToken (TOKEN_MINUS);
    case '+':
        return makeToken (TOKEN_PLUS);

    case ';':
        return makeToken (TOKEN_SEMICOLON);
    case '/':
        return makeToken (TOKEN_SLASH);
    case '*':
        return makeToken (TOKEN_STAR);
    }

    return errorToken ("Unexpected character.");
}

static bool
isAtEnd (
    )
{
    return *scanner.current == '\0';
}

static char
advance (
    )
{
    return *scanner.current++;
}

static Token
makeToken (
    TokenType type)
{
    Token token;

    token.type = type;

    token.start = scanner.start;
    token.length = scanner.current - scanner.start;
    token.line = scanner.line;
    return token;
}

static Token
errorToken (
    const char *message)
{
    Token token;

    token.type = TOKEN_ERROR;

    token.start = message;
    token.length = strlen (message);
    token.line = scanner.line;
    return token;
}
