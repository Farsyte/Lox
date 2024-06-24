#include "keyword.h"

#include "assert.h"
#include "token.h"

#include <stdio.h>
#include <string.h>

#define N(a)	(sizeof a / sizeof a[0])

static struct toktable {
    TokenType tt;
    const char *str;
} tt[] = {
    {TOKEN_AND, "andxyz"},
    {TOKEN_BREAK, "breakxyz"},
    {TOKEN_CLASS, "classxyz"},
    {TOKEN_ELSE, "elsexyz"},
    {TOKEN_IF, "ifxyz"},
    {TOKEN_NIL, "nilxyz"},
    {TOKEN_OR, "orxyz"},
    {TOKEN_PRINT, "printxyz"},
    {TOKEN_RETURN, "returnxyz"},
    {TOKEN_SUPER, "superxyz"},
    {TOKEN_VAR, "varxyz"},
    {TOKEN_WHILE, "whilexyz"},
    {TOKEN_FALSE, "falsexyz"},
    {TOKEN_FOR, "forxyz"},
    {TOKEN_FUN, "funxyz"},
    {TOKEN_THIS, "thisxyz"},
    {TOKEN_TRUE, "truexyz"},

};

static int ntt = N (tt);

void
bist_keyword (
    )
{
    char cbuf[512];
    char mbuf[512];

    assert (TOKEN_IDENTIFIER == keyword_token_type ("", 0),
        "Verify the degenerate case: '' should be TOKEN_IDENTIFER.");
    assert (TOKEN_IDENTIFIER == keyword_token_type ("_", 1),
        "Verify a trivial case: '_' should be TOKEN_IDENTIFER.");

    for (int i = 0; i < ntt; ++i) {
        int l = strlen (tt[i].str) - 3;

        for (int cw = l + 2; cw > 0; --cw) {
            TokenType exp = (cw == l) ? tt[i].tt : TOKEN_IDENTIFIER;

            TokenType obs = keyword_token_type (tt[i].str, cw);

            if (exp == obs)
                continue;

            sprintf (cbuf, "keyword_token_type(\"%s\", %d) // '%.*s'",
                tt[i].str, cw, cw, tt[i].str);

            sprintf (mbuf, "Expected result: %s\nObserved result: %s",
                token_type_name (exp), token_type_name (obs));

            build_msg ("Assertion Failed", cbuf, mbuf);
            exit (1);
        }
    }
}
