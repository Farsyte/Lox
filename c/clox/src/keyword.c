#include "keyword.h"

#include "assert.h"

#include <stdio.h>
#include <string.h>

TokenType
keyword_token_type (
    const char *t,
    int n)
{
    assert (NULL != t, "keyword_token_type recevied a NULL t pointer.");

    // this set of nested switch statements looks a bit odd
    // but is expected to be blazingly fast.
    //
    // I hoisted the work into a macro and included the "case" line
    // so my indenter does not double the length of this thing.

    if (n > 1) {                // all keywords are at least two characers.
        switch (t[0]) {

#define CASE_RETURN_MAYBE(c,o,clen,r,tok)		\
	    case c:					\
		if ((n - o == sizeof r - 1) &&		\
		    (0 == strncmp (t+o, r, n-o)))  	\
		    return tok;				\
		break

            CASE_RETURN_MAYBE ('a', 1, 2, "nd", TOKEN_AND);
            CASE_RETURN_MAYBE ('b', 1, 4, "reak", TOKEN_BREAK);
            CASE_RETURN_MAYBE ('c', 1, 4, "lass", TOKEN_CLASS);
            CASE_RETURN_MAYBE ('e', 1, 3, "lse", TOKEN_ELSE);
            CASE_RETURN_MAYBE ('i', 1, 1, "f", TOKEN_IF);
            CASE_RETURN_MAYBE ('n', 1, 2, "il", TOKEN_NIL);
            CASE_RETURN_MAYBE ('o', 1, 1, "r", TOKEN_OR);
            CASE_RETURN_MAYBE ('p', 1, 4, "rint", TOKEN_PRINT);
            CASE_RETURN_MAYBE ('r', 1, 5, "eturn", TOKEN_RETURN);
            CASE_RETURN_MAYBE ('s', 1, 4, "uper", TOKEN_SUPER);
            CASE_RETURN_MAYBE ('v', 1, 2, "ar", TOKEN_VAR);
            CASE_RETURN_MAYBE ('w', 1, 4, "hile", TOKEN_WHILE);

        case 'f':
            switch (t[1]) {
                CASE_RETURN_MAYBE ('a', 2, 3, "lse", TOKEN_FALSE);
                CASE_RETURN_MAYBE ('o', 2, 1, "r", TOKEN_FOR);
                CASE_RETURN_MAYBE ('u', 2, 1, "n", TOKEN_FUN);
            }
            break;

        case 't':
            switch (t[1]) {
                CASE_RETURN_MAYBE ('h', 2, 2, "is", TOKEN_THIS);
                CASE_RETURN_MAYBE ('r', 2, 2, "ue", TOKEN_TRUE);
            }
            break;
        }
    }

    return TOKEN_IDENTIFIER;
}
