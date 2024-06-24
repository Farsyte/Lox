#include "scanner.h"

#include "assert.h"
#include "common.h"
#include "keyword.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool isAtEnd (
    );
static char advance (
    );
static bool match (
    char expected);
static Token makeToken (
    TokenType type);
static Token errorToken (
    const char *message);
static void skipWhitespace (
    );
static Token identifier (
    );
static Token number (
    );
static Token string (
    );
static char peek (
    );
static char peekNext (
    );

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
    skipWhitespace ();

    scanner.start = scanner.current;
    if (isAtEnd ())
        return makeToken (TOKEN_EOF);

    char c = advance ();

    if (isalpha (c) || (c == '_'))
        return identifier ();
    if (isdigit (c))
        return number ();

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

    case '!':
        return makeToken (match ('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return makeToken (match ('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
        return makeToken (match ('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
        return makeToken (match ('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

    case '"':
        return string ();
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

static bool
match (
    char expected)
{
    if (isAtEnd ())
        return false;
    if (*scanner.current != expected)
        return false;
    scanner.current++;
    return true;
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

static void
skipWhitespace (
    )
{
    for (;;) {
        char c = peek ();

        switch (c) {

        case ' ':
        case '\r':
        case '\t':
            advance ();
            break;

        case '\n':
            scanner.line++;
            advance ();
            break;

        case '/':
            if (peekNext () == '/') {
                // A comment goes until the end of the line.
                while (peek () != '\n' && !isAtEnd ())
                    advance ();
            } else {
                return;
            }
            break;

        default:
            return;
        }
    }
}

static Token
identifier (
    )
{
    char ch = peek ();

    while (isalnum (ch) || (ch == '_')) {
        advance ();
        ch = peek ();
    }
    return makeToken (keyword_token_type (scanner.start,
            scanner.current - scanner.start));
}

static Token
number (
    )
{
    while (isdigit (peek ()))
        advance ();

    // Allow a fractional part.
    if (peek () == '.' && isdigit (peekNext ())) {
        // consume the decimal point.
        advance ();

        while (isdigit (peek ()))
            advance ();
    }

    return makeToken (TOKEN_NUMBER);
}

static Token
string (
    )
{
    while (peek () != '"' && !isAtEnd ()) {
        if (peek () == '\n')
            scanner.line++;
        advance ();
    }

    if (isAtEnd ())
        return errorToken ("Unterminated String.");

    // the closing quote.
    advance ();
    return makeToken (TOKEN_STRING);
}

static char
peek (
    )
{
    return scanner.current[0];
}

static char
peekNext (
    )
{
    return scanner.current[1];
}
