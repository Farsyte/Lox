#include <stdio.h>
#include "list.h"

int main(int argc, char **argv)
{

    (void)argc;         /* intentionally not used */
    (void)argv;         /* intentionally not used */

    printf("Hello C!\n");

    list_test();
    return 0;
}