#include "main.h"

#include "common.h"

#include <stdio.h>
#include <string.h>

int
main (
    int argc,
    const char **argv)
{
    post_all ();

    if (argc < 1)
        STUB ("interpret stdin");
    else if (!strcmp (argv[1], "bist"))
        bist_all ();
    else
        STUB ("interpret file");

    return 0;
}
