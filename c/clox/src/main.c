#include "assert.h"
#include "bist.h"
#include "common.h"
#include "post.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

static void repl (
    );
static void runFile (
    const char *path);
static char *readFile (
    const char *path);

int
main (
    int argc,
    const char *argv[])
{
    post_all ();

    // Special case: do not init the VM before BIST runs.

    if ((argc == 2) && (!strcmp (argv[1], "bist"))) {
        bist_all ();
    }

    initVM ();

    if (argc == 1) {
        repl ();
    } else if (argc == 2) {
        runFile (argv[1]);
    } else {
        fprintf (stderr, "Usage: clox [bist | <path>]\n");
    }

    freeVM ();

    return 0;
}

static void
repl (
    )
{
    char line[1024];

    for (;;) {
        printf ("> ");
        if (!fgets (line, sizeof line, stdin)) {
            printf ("\n");
            break;
        }

        interpret (line);
    }
}

static void
runFile (
    const char *path)
{
    char *source = readFile (path);
    InterpretResult result = interpret (source);

    free (source);

    if (result == INTERPRET_COMPILE_ERROR)
        exit (65);
    if (result == INTERPRET_RUNTIME_ERROR)
        exit (70);
}

static char *
readFile (
    const char *path)
{
    FILE *file = fopen (path, "rb");

    if (file == NULL) {
        fprintf (stderr, "Could not open file \"%s\".\n", path);
        exit (74);
    }

    fseek (file, 0L, SEEK_END);
    size_t fileSize = ftell (file);

    rewind (file);

    char *buffer = malloc (fileSize + 1);

    if (file == NULL) {
        fprintf (stderr, "Not enough memory to read \"%s\".\n", path);
        exit (74);
    }

    size_t bytesRead = fread (buffer, sizeof (char), fileSize, file);

    if (bytesRead != fileSize) {
        fprintf (stderr, "Could not read file \"%s\".\n", path);
        exit (74);
    }

    buffer[bytesRead] = '\0';

    fclose (file);
    return buffer;
}
