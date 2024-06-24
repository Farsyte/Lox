#include "scanner.h"

#include "assert.h"
#include "bist.h"

#include <stdio.h>
#include <string.h>

// define SCANNER_BIST_DEBUG_TRACE if bist_scanner reports
// an error and you can't find it.
// #define SCANNER_BIST_DEBUG_TRACE

static void checkScanner (
    const char *source,
    const TokenType expected_type[]);

void
bist_scanner (
    )
{
    {
        // check the degenerate case first.
        const char source[] = "";

        const TokenType expected_type[] = {
            TOKEN_EOF
        };

        checkScanner (source, expected_type);
    }

    {
        // check single-character tokens.
        const char source[] = "(){},.-+;/*";

        const TokenType expected_type[] = {
            TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
            TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
            TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
            TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
            TOKEN_EOF
        };

        checkScanner (source, expected_type);
    }

    {
        // check two-character tokens.
        const char source[] = "!!====<<=>>=";

        const TokenType expected_type[] = {
            TOKEN_BANG, TOKEN_BANG_EQUAL,
            TOKEN_EQUAL_EQUAL, TOKEN_EQUAL,
            TOKEN_LESS, TOKEN_LESS_EQUAL,
            TOKEN_GREATER, TOKEN_GREATER_EQUAL,
            TOKEN_EOF
        };

        checkScanner (source, expected_type);
    }

    {
        // Whitespace Sanity Check
        const char source[] = "! !=\t= ==\n< <=\t> >=\n";

        const TokenType expected_type[] = {
            TOKEN_BANG, TOKEN_BANG_EQUAL,
            TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
            TOKEN_LESS, TOKEN_LESS_EQUAL,
            TOKEN_GREATER, TOKEN_GREATER_EQUAL,
            TOKEN_EOF
        };

        checkScanner (source, expected_type);
    }

    {
        // Comment Sanity Check
        const char source[] =
            "// Komments Я Kool!\n! !=\t= ==// equalities\n< <=\t> >=// inequalities\n";

        const TokenType expected_type[] = {
            TOKEN_BANG, TOKEN_BANG_EQUAL,
            TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
            TOKEN_LESS, TOKEN_LESS_EQUAL,
            TOKEN_GREATER, TOKEN_GREATER_EQUAL,
            TOKEN_EOF
        };

        checkScanner (source, expected_type);
    }

    {
        // Strings, too.
        const char source[] =
            "// Line two is a string\n"
            "\"This is a string.\"\n"
            "// Line four starts a multiline string.\n"
            "\"This string // this is not a comment\n"
            "extends across two lines.\"\n";

        const TokenType expected_type[] = {
            TOKEN_STRING,
            TOKEN_STRING,
            TOKEN_EOF
        };

        checkScanner (source, expected_type);
    }

    {
        // Numbers
        // If there is a decimal point, there has to be
        // at least one digit on either side.
        // The scanner stores the lexeme.
        // It does not convert it to a numeric value (yet).
        // It does not support "1.0e6" (yet).
        // It does not support alternate bases (yet).
        const char source[] =
            "0		// an integer value\n"
            "00		// an integer value\n"
            "123	// an integer value\n"
            "123.	// an integer value and a dot\n"
            ".123	// a dot and an integer value \n"
            "123.123	// a floating point value";

        const TokenType expected_type[] = {
            TOKEN_NUMBER,
            TOKEN_NUMBER,
            TOKEN_NUMBER,
            TOKEN_NUMBER, TOKEN_DOT,
            TOKEN_DOT, TOKEN_NUMBER,
            TOKEN_NUMBER,
            TOKEN_EOF
        };

        checkScanner (source, expected_type);
    }
}

static void
checkScanner (
    const char *source,
    const TokenType expected_type[])
{
    initScanner (source);

    int line = 1;
    Token t;

    int off = 0;
    int len = strlen (source);

#ifdef SCANNER_BIST_DEBUG_TRACE
    printf ("\n");
    printf ("scanner debug trace ...\n");
    printf ("vvv\n%s\n^^^\n", source);
#endif

    for (int i = 0; expected_type[i] != TOKEN_EOF; ++i) {
        t = scanToken ();

        assert (expected_type[i] == t.type,
            "scanToken returned a wrong token type");

        assert (source + off <= t.start,
            "scanToken should correctly report the start of the token.");

#ifdef SCANNER_BIST_DEBUG_TRACE
        if (source + off < t.start) {
            printf ("skip over '%.*s'\n",
                (int) (t.start - (source + off)), source + off);
#endif
            while (source + off < t.start) {
                if (source[off++] == '\n') {
                    line++;
#ifdef SCANNER_BIST_DEBUG_TRACE
                    printf
                        ("advanced across newline at offset %d, starting line %d\n",
                        off - 1, line);
#endif
                }
            }
#ifdef SCANNER_BIST_DEBUG_TRACE
            printf ("expected line is now %d (t.line is %d)\n", line, t.line);
        }
#endif

        assert (0 < t.length,
            "scanToken should tell us the token length is positive.");

#ifdef SCANNER_BIST_DEBUG_TRACE
        if (t.length > 0) {
            printf ("consuming lexeme '%.*s'\n", t.length, t.start);
#endif
            for (int i = t.length; i > 0; i--) {
                if (source[off++] == '\n') {
                    line++;
#ifdef SCANNER_BIST_DEBUG_TRACE
                    printf
                        ("advanced across newline at offset %d, starting line %d\n",
                        off - 1, line);
#endif
                }
            }
#ifdef SCANNER_BIST_DEBUG_TRACE
            printf ("expected line is now %d\n", line);
        }
#endif

        assert (line == t.line, "scanToken should tell us the line number.");

        assert (off <= len, "scanToken advanced us past the EOF marker");

#ifdef SCANNER_BIST_DEBUG_TRACE
        printf ("token %04d %-24s returned for '%.*s'\n",
            i, token_type_name (t.type), t.length, t.start);
#endif
    }

#ifdef SCANNER_BIST_DEBUG_TRACE
    printf ("scanner debug trace ... end.\n");
    printf ("\n");
#endif

    // make sure the 1st TOKEN_EOF is correct.

    t = scanToken ();

    assert (TOKEN_EOF == t.type,
        "scanToken did not return TOKEN_EOF when expected");
    assert (source + off <= t.start,
        "scanToken should correctly report the position of the EOF.");

#ifdef SCANNER_BIST_DEBUG_TRACE
    if (source + off < t.start)
        printf ("advancing over '%.*s'\n",
            (int) (t.start - (source + off)), source + off);
#endif
    while (source + off < t.start) {
        if (source[off++] == '\n') {
            line++;
#ifdef SCANNER_BIST_DEBUG_TRACE
            printf
                ("advanced across newline at offset %d, starting line %d\n",
                off, line);
#endif
        }
    }

    assert (0 == t.length,
        "scanToken should tell us the TOKEN_EOF length is zero.");
    assert (line == t.line, "scanToken should tell us the line number.");
    assert (off == len, "scanToken did not advance us to the EOF marker");

    // make sure the 2nd TOKEN_EOF is correct.

    t = scanToken ();

    assert (TOKEN_EOF == t.type,
        "scanToken did not return TOKEN_EOF when expected");
    assert (source + off == t.start,
        "scanToken should correctly report the position of the EOF.");
    assert (0 == t.length,
        "scanToken should tell us the TOKEN_EOF length is zero.");
    assert (line == t.line, "scanToken should tell us the line number.");
    assert (off == len, "scanToken did not advance us to the EOF marker");

}
