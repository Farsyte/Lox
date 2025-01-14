#include "main.h"

#include "bist.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "memory.h"
#include "options.h"
#include "post.h"
#include "vm.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

/** @file main.c
 * @brief Implementation of Main Function
 *
 * This file provides the main entry point as defined by the C
 * programming langage, and contains the logic driving cLox based on
 * the command line arguments presented.
 */

extern Options options;         // share command line options state

/** Read-Evaluate-Print loop.
 */
static void
repl ()
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

/** Read the content of a file into a string.
 *
 * @param path where to find the file
 * @returns the content of the file, plus a '\0' terminator.
 *
 * Any failure will terminate the program abnormally with
 * an appropriate error message.
 */
static char *
readFile (const char *path)
{
    FILE *file = fopen (path, "rb");

    if (NULL == file) {
        int en = errno;

        // *INDENT-OFF*
        fprintf (stderr,
                 "Could not open file \"%s\".\n"
                 "    fopen returned error %d: %s\n",
                 path, en, strerror (en));
        // *INDENT-ON*

        exit (EX_NOINPUT);
    }

    fseek (file, 0L, SEEK_END);
    size_t fileSize = ftell (file);

    rewind (file);

    char *buffer = ALLOCATE (char, fileSize + 1);       // was (char *) malloc (fileSize + 1);

    if (NULL == buffer) {
        int en = errno;

        // *INDENT-OFF*
        fprintf (stderr,
                 "Not enough memory to read %lu bytes from \"%s\".\n"
                 "   malloc returned error number %d: %s\n",
                 fileSize, path, en, strerror (en));
        // *INDENT-ON*

        // try again later when more memory is available.
        exit (EX_TEMPFAIL);
    }
    size_t bytesRead = fread (buffer, sizeof (char), fileSize, file);

    if (bytesRead != fileSize) {

        // *INDENT-OFF*
        fprintf (stderr,
                 "Could not read %lu bytes from file \"%s\"\n",
                 fileSize, path);
        // *INDENT-ON*

        exit (EX_IOERR);
    }
    buffer[bytesRead] = '\0';

    fclose (file);
    return buffer;
}

/** Process the Lox source in the file named.
 *
 * @param path where to find the file
 *
 * On error, terminates the program.
 */
static void
runFile (const char *path)
{
    char *source = readFile (path);
    InterpretResult result = interpret (source);

    FREE_ARRAY (char, (char *) source, 1 + strlen (source));    // was free (source);

    if (result == INTERPRET_COMPILE_ERROR)
        exit (EX_DATAERR);
    if (result == INTERPRET_RUNTIME_ERROR)
        exit (EX_SOFTWARE);
}

/** Demonstration Function
 *
 * This function contains whatever demonstration code was most
 * recently used by the book to show some code working, and is
 * cleared when that code is no longer interesting to demo.
 */
static void
demo ()
{
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
 * @returns exit status values defined in "sysexits.h"
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
main (int argc, const char **argv)
{
    postAll ();

    initMemory ();
    initVM ();

    options.bist = bistAll;
    options.demo = demo;
    options.cfar = runFile;
    options.repl = repl;

    parse_options (argc, argv);

    freeVM ();
    return 0;
}
