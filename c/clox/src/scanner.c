#include "scanner.h"

#include "common.h"

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

Token
scanToken (
    )
{
    skipWhitespace ();
    scanner.start = scanner.current;

    if (isAtEnd ())
        return makeToken (TOKEN_EOF);

    char c = advance ();

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

        // *INDENT-ON*

    }

    return errorToken ("Unexpected character.");
}
