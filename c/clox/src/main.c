#include "bist.h"
#include "common.h"
#include "post.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

int
main (
    int argc,
    const char *argv[])
{
    post_all ();

    if ((argc == 2) && (!strcmp (argv[1], "bist")))
        bist_all ();

    initVM ();

    freeVM ();

    return 0;
}
