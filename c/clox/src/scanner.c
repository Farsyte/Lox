#include "scanner.h"

#include "common.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

Scanner scanner;                ///< Storage for scanner state

/** Initialize the scanne
 *
 * @param source start looking at this text
 */
void
initScanner (const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

/** Does Lox consider this characer to be a Digit?
 * @param c the character asked about
 * @return true if in 0..9, else false.
 */
static bool
isDigit (char c)
{
    return (c >= '0') && (c <= '9');
}

/** Does Lox consider this characer to be Alpha?
 * @param c the character asked about
 * @return true if in A-Z, a-z, or _; else false.
 */
static bool
isAlpha (char c)
{
    // *INDENT-OFF*
    return
        ((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z')) ||
        (c == '_');
    // *INDENT-ON*
}

/** Does Lox consider this characer to be AlphaNumeric?
 * @param c the character asked about
 * @return true if in 0-9, A-Z, a-z, or _; else false.
 */
static bool
isAlNum (char c)
{
    // *INDENT-OFF*
    return
        ((c >= '0') && (c <= '9')) ||
        ((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z')) ||
        (c == '_');
    // *INDENT-ON*
}

/** Are we at the end of the file?
 *
 * This function returns true, if and only if attempting to scan
 * another token would start at the end of the source.
 *
 * @return true if the scanner is looking at the end of the source.
 */
static bool
isAtEnd ()
{
    return *scanner.current == '\0';
}

/** Advance to the next source character.
 *
 * Any '\0' encountered is "sticky" -- it is returned, and
 * the scanner is not advanced.
 *
 * If a '\n' is encountered, the lione number is incremented.
 *
 * @return the next character in the source.
 */
static char
advance ()
{
    int ch = *scanner.current;

    if (ch) {
        scanner.current++;
        if (ch == '\n')
            scanner.line++;
    }
    return ch;
}

/** Peek at the next source character.
 *
 * Does not advance the scanner.
 *
 * @return the next character under the scan head.
 */
static char
peek ()
{
    return *scanner.current;
}

/** Peek at the 2nd next source character.
 *
 * Does not advance the scanner. Returns not the next
 * character in the source but the one after that.
 *
 * @return the 2nd next character under the scan head.
 */
static char
peekNext ()
{
    if (isAtEnd ())
        return '\0';
    return scanner.current[1];
}

/** Match a character from the source.
 *
 * If the next character in the source matches the one
 * expected, advance the scanner and return true; else
 * return false (includes the "end of file" case).
 *
 * @param expected what the caller is expecting to see.
 * @return true if matched, else false.
 */
static bool
match (char expected)
{
    if (isAtEnd ())
        return false;
    if (*scanner.current != expected)
        return false;
    scanner.current++;
    return true;
}

/** Construct a token from the scanned sequence.
 *
 * The source text from start to current is a token. Wrap it
 * up in a Token structure with the provided type, and log
 * the current line number.
 *
 * @param type the type for the token
 * @return a Token structure
 */
static Token
makeToken (TokenType type)
{
    Token token;

    token.type = type;
    token.start = scanner.start;
    token.length = (int) (scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

/** Construct an error token from the scanned sequence.
 *
 * Construct a Token of type TOKEN_ERROR, with the lexeme
 * text set to the provided message.
 *
 * @param message a message to put in the token.
 * @return a Token of type TOKEN_ERROR.
 */
static Token
errorToken (const char *message)
{
    Token token;

    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int) strlen (message);
    token.line = scanner.line;
    return token;
}

/** Scan forward across whitespace.
 *
 * Scan forward across spaces, tabs, returns, newlines,
 * and comments. Do not consume the first non-whitespace
 * character (which does not itself introduce a comment).
 */
static void
skipWhitespace ()
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

            if (peekNext () == '/') {   // treat "//" as comment to end of line.
                for (;;) {
                    c = advance ();
                    if (c == '\0')
                        return;
                    if ('\n' == c)
                        break;
                }
                break;
            }

            if (peekNext () == '*') {   // treat "/*" as comment to "*/"
                advance ();             // the slash
                advance ();             // the star
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

/** Finish the keyword comparison for this lexeme.
 *
 * This function is called for the portion of the lexeme not
 * already checked, to see if the whole token is the keyword
 * implied by what has already been checked. If the lexeme
 * matches, return the provided token type; if not, then
 * return the TOKEN_IDENTIFIER type.
 *
 * @param start the offset into the lexeme for comparison
 * @param length the length to compare
 * @param rest the string to compare to the rest of the lexeme
 * @param type provides the default token type
 * @return provided type on a match, or TOKEN_IDENTIFIER if not.
 */
static TokenType
checkKeyword (int start, int length, const char *rest, TokenType type)
{
    if (scanner.current - scanner.start == start + length && memcmp (scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

/** Determine the token type for an identifier.
 *
 * Examines it swiftly to determine if it is a Lox keyword,
 * and if so, return the correct token type. If not, return
 * the TOKEN_IDENTIFIER token type.
 *
 * @return TOKEN_IDENTIFIER or the token type for a keyword
 */
static TokenType
identifierType ()
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

/** Finish scanning an identifier.
 *
 * The scanner has decided it is looking at an identifier,
 * finish consuming it, determine the correct token type,
 * then construct an appropriate token.
 *
 * @return a new Token (not a pointer).
 */
static Token
identifier ()
{
    while (isAlNum (peek ()))
        advance ();
    return makeToken (identifierType ());
}

/** Finish scanning a number.
 *
 * The scanner has decided it is looking at a number,
 * finish consuming it, and create the Token.
 *
 * @return a new Token (not a pointer).
 */
static Token
number ()
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

/** Finish scanning a string.
 *
 * The scanner has decided it is looking at a string,
 * finish consuming it, and create the Token.
 *
 * @return a new Token (not a pointer).
 */
static Token
string ()
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

/** Scan the next token.
 *
 * @return a new Token (not a pointer).
 */
Token
scanToken ()
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
