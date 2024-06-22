#include "bist.h"

#include "assert.h"
#include "chunk.h"

#include <stdio.h>              /* fprintf, stderr */
#include <string.h>             /* strcmp */

void
bist_all (
    )
{
    bist_chunk ();
    exit (EXIT_SUCCESS);
}
