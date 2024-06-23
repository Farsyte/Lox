#include "scanner.h"

#include "assert.h"
#include "bist.h"

#include <stdio.h>
#include <string.h>

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

    for (int i = 0; expected_type[i] != TOKEN_EOF; ++i) {
        t = scanToken ();

        assert (expected_type[i] == t.type,
            "scanToken returned a wrong token type");

        assert (source + off <= t.start,
            "scanToken should correctly report the start of the token.");

        // TODO update expected line number
        // when advancing "off"
        off = t.start - source;

        assert (0 < t.length,
            "scanToken should tell us the token length is positive.");

        assert (line == t.line, "scanToken should tell us the line number.");

        // TODO update expected line number
        // when advancing "off"
        off += t.length;

        assert (off <= len, "scanToken advanced us past the EOF marker");
    }

    // make sure the 1st TOKEN_EOF is correct.

    t = scanToken ();

    assert (TOKEN_EOF == t.type,
        "scanToken did not return TOKEN_EOF when expected");
    assert (source + off <= t.start,
        "scanToken should correctly report the position of the EOF.");

    // TODO update expected line number
    // when advancing "off"
    off = t.start - source;

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
