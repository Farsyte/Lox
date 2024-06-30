#include "scanner.h"

#include "common.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Does any code outside scanner.c have any reason
// to look inside the Scanner structure?

Scanner scanner;

void
initScanner (
    const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool
isDigit (
    char c)
{
    return (c >= '0') && (c <= '9');
}

static bool
isAlpha (
    char c)
{
    // *INDENT-OFF*
    return
        ((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z')) ||
        (c == '_');
    // *INDENT-ON*
}

static bool
isAlNum (
    char c)
{
    // *INDENT-OFF*
    return
        ((c >= '0') && (c <= '9')) ||
        ((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z')) ||
        (c == '_');
    // *INDENT-ON*
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
    int ch = *scanner.current;

    if (ch) {
        scanner.current++;
        if (ch == '\n')
            scanner.line++;
    }
    return ch;
}

static char
peek (
    )
{
    return *scanner.current;
}

static char
peekNext (
    )
{
    if (isAtEnd ())
        return '\0';
    return scanner.current[1];
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
    token.length = (int) (scanner.current - scanner.start);
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
    token.length = (int) strlen (message);
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
        case '\n':
            advance ();
            break;

        case '/':

            if (peekNext () == '/') {
                // treat "//" as comment to end of line.
                for (;;) {
                    c = advance ();
                    if (c == '\0')
                        return;
                    if ('\n' == c)
                        break;
                }
                break;
            }

            if (peekNext () == '*') {
                // treat "/*" as comment to "*/"
                advance ();     // the slash
                advance ();     // the star
                for (;;) {
                    c = advance ();
                    if (c == '\0')
                        return;
                    if (('*' == c) && ('/' == advance ()))
                        break;
                }
                break;
            }
            // anything else starting with '/' is not whitespace.
            return;

        default:
            return;
        }
    }
}

static TokenType
checkKeyword (
    int start,
    int length,
    const char *rest,
    TokenType type)
{
    if (scanner.current - scanner.start == start + length &&
        memcmp (scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType
identifierType (
    )
{

    switch (scanner.start[0]) {

        // *INDENT-OFF*

    case 'a': return checkKeyword (1, 2, "nd", TOKEN_AND);
    case 'c': return checkKeyword (1, 4, "lass", TOKEN_CLASS);
    case 'e': return checkKeyword (1, 3, "lse", TOKEN_ELSE);
    case 'i': return checkKeyword (1, 1, "f", TOKEN_IF);
    case 'n': return checkKeyword (1, 2, "il", TOKEN_NIL);
    case 'o': return checkKeyword (1, 1, "r", TOKEN_OR);
    case 'p': return checkKeyword (1, 4, "rint", TOKEN_PRINT);
    case 'r': return checkKeyword (1, 5, "eturn", TOKEN_RETURN);
    case 's': return checkKeyword (1, 4, "uper", TOKEN_SUPER);
    case 'v': return checkKeyword (1, 2, "ar", TOKEN_VAR);
    case 'w': return checkKeyword (1, 4, "hile", TOKEN_WHILE);

    case 'f':
        if (scanner.current - scanner.start > 1) {
            switch (scanner.start[1]) {
            case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
            case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
            case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
            }
        }
        break;

    case 't':
        if (scanner.current - scanner.start > 1) {
            switch (scanner.start[1]) {
            case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
            case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
            }
        }
        break;

        // *INDENT-ON*

    }

    return TOKEN_IDENTIFIER;
}

static Token
identifier (
    )
{
    while (isAlNum (peek ()))
        advance ();
    return makeToken (identifierType ());
}

static Token
number (
    )
{
    // consume the rest of the integer part.
    while (isDigit (peek ()))
        advance ();

    // Look for a fractional part.
    if (peek () == '.' && isDigit (peekNext ())) {
        // consume the "."
        advance ();

        // consume the fractional part
        while (isDigit (peek ()))
            advance ();
    }

    return makeToken (TOKEN_NUMBER);
}

static Token
string (
    )
{
    // consume through the closing quote.
    while ('"' != advance ()) {
        if (isAtEnd ()) {
            return errorToken ("Unterminated String.");
        }
    }

    // NOTE: if the string contains a newline,
    // the line number in the token is the number
    // of the line the string ENDS in.

    return makeToken (TOKEN_STRING);
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

    if (isDigit (c))
        return number ();
    if (isAlpha (c))
        return identifier ();

    switch (c) {

        // *INDENT-OFF*

    case '(': return makeToken (TOKEN_LEFT_PAREN);
    case ')': return makeToken (TOKEN_RIGHT_PAREN);
    case '{': return makeToken (TOKEN_LEFT_BRACE);
    case '}': return makeToken (TOKEN_RIGHT_BRACE);
    case ';': return makeToken (TOKEN_SEMICOLON);
    case ',': return makeToken (TOKEN_COMMA);
    case '.': return makeToken (TOKEN_DOT);
    case '-': return makeToken (TOKEN_MINUS);
    case '+': return makeToken (TOKEN_PLUS);
    case '/': return makeToken (TOKEN_SLASH);
    case '*': return makeToken (TOKEN_STAR);

    case '!': return makeToken (match('=') ? TOKEN_BANG_EQUAL    : TOKEN_BANG   );
    case '=': return makeToken (match('=') ? TOKEN_EQUAL_EQUAL   : TOKEN_EQUAL  );
    case '<': return makeToken (match('=') ? TOKEN_LESS_EQUAL    : TOKEN_LESS   );
    case '>': return makeToken (match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

    case '"': return string();

        // *INDENT-ON*

    }

    return errorToken ("Unexpected character.");
}
