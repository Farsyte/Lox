#include "main.h"

#include "bist.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "options.h"
#include "post.h"
#include "vm.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

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

static char *
readFile (
    const char *path)
{
    FILE *file = fopen (path, "rb");

    if (NULL == file) {
        int en = errno;

        fprintf (stderr, "Could not open file \"%s\".\n"
            "    fopen returned error %d: %s\n", path, en, strerror (en));
        exit (EX_NOINPUT);
    }

    fseek (file, 0L, SEEK_END);
    size_t fileSize = ftell (file);

    rewind (file);

    char *buffer = (char *) malloc (fileSize + 1);

    if (NULL == buffer) {
        int en = errno;

        fprintf (stderr,
            "Not enough memory to read %lu bytes from \"%s\".\n"
            "   malloc returned error number %d: %s\n",
            fileSize, path, en, strerror (en));
        // try again later when more memory is available.
        exit (EX_TEMPFAIL);
    }
    size_t bytesRead = fread (buffer, sizeof (char), fileSize, file);

    if (bytesRead != fileSize) {
        fprintf (stderr, "Could not read %lu bytes from file \"%s\"\n",
            fileSize, path);
        exit (EX_IOERR);
    }
    buffer[bytesRead] = '\0';

    fclose (file);
    return buffer;
}

static void
runFile (
    const char *path)
{
    char *source = readFile (path);
    InterpretResult result = interpret (source);

    free (source);

    if (result == INTERPRET_COMPILE_ERROR)
        exit (EX_DATAERR);
    if (result == INTERPRET_RUNTIME_ERROR)
        exit (EX_SOFTWARE);
}

/** @file main.c
 * @brief Implementation of Main Function
 *
 * This file provides the main entry point as defined by the C
 * programming langage, and contains the logic driving cLox based on
 * the command line arguments presented.
 */

static void
demo (
    )
{
    initVM ();

    Chunk chunk;
    int constant;

    initChunk (&chunk);

    constant = addConstant (&chunk, 1.2);
    writeChunk (&chunk, OP_CONSTANT, 123);
    writeChunk (&chunk, constant, 123);

    constant = addConstant (&chunk, 3.4);
    writeChunk (&chunk, OP_CONSTANT, 123);
    writeChunk (&chunk, constant, 123);

    writeChunk (&chunk, OP_ADD, 123);

    constant = addConstant (&chunk, 5.6);
    writeChunk (&chunk, OP_CONSTANT, 123);
    writeChunk (&chunk, constant, 123);

    writeChunk (&chunk, OP_DIVIDE, 123);

    writeChunk (&chunk, OP_NEGATE, 123);

    writeChunk (&chunk, OP_RETURN, 124);
    disassembleChunk (&chunk, "test chunk");

    interpretChunk (&chunk);

    freeChunk (&chunk);

    freeVM ();
}

/** Main Entry Point.
 *
 * Usage:
 *     clox [options]
 * 
 * Options:
 *     Options are scanned strictly left to right, changing flags
 *     or exercising bits of cLox. If no file names are specified
 *     and the REPL was not explicitly run, it will be automatically
 *     run after parsing is complete.
 * 
 * Special Options:
 *     -                   (with no short flags) run the REPL.
 *     --                  The rest of the arguments are all filenames.
 * 
 * Flags:
 *     -?, -h, --help      produce this help message
 *     -v, --verbose       turn up verbose operation flag
 *     -n, --dryrun        turn up dryrun flag
 *     -d, --debug         turn up debug level
 *     -V, --no-verbose    turn down verbose operation flag
 *     -N, --no-dryrun     turn down dryrun flag
 *     -D, --no-debug      turn down debug level
 *         --bist          run the built-in self test
 *         --demo          run the quick demo code
 * 
 *     Multiple short flags can be combined (so -dv will turn up both the
 *     debug and verbose levels).
 *
 * @param argc command line arg count
 * @param argv command line arg list
 * @return exit status values defined in "sysexits.h"
 *
 * Exit Status Values:
 * - 0: successful termination (EX_OK)
 * - 64: command line usage error (EX_USAGE)
 * - 65: data format error (EX_DATAERR)
 * - 66: cannot open input (EX_NOINPUT)
 * - 67: addressee unknown (EX_NOUSER)
 * - 68: host name unknown (EX_NOHOST)
 * - 69: service unavailable (EX_UNAVAILABLE)
 * - 70: internal software error (EX_SOFTWARE)
 * - 71: system error (e.g., can't fork) (EX_OSERR)
 * - 72: critical OS file missing (EX_OSFILE)
 * - 73: can't create (user) output file (EX_CANTCREAT)
 * - 74: input/output error (EX_IOERR)
 * - 75: temp failure; user is invited to retry (EX_TEMPFAIL)
 * - 76: remote error in protocol (EX_PROTOCOL)
 * - 77: permission denied (EX_NOPERM)
 * - 78: configuration error (EX_CONFIG)
 */
int
main (
    int argc,
    const char **argv)
{
    postAll ();

    initVM ();

    options.bist = bistAll;
    options.demo = demo;
    options.cfar = runFile;
    options.repl = repl;

    parse_options (argc, argv);

    freeVM ();
    return 0;
}
