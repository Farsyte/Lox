#include "options.h"

#include "common.h"
#include "string.h"

#include <stdio.h>

/** @file options.c
 * @brief manage command line options
 */

Options options;                ///< Storage for command line options.

/** the text to be printed by the --help option.
 */
static const char *help_text[] = {
    "",
    "Usage:",
    "    clox [options]",
    "",
    "Options:",
    "    Options are scanned strictly left to right, changing flags",
    "    or exercising bits of cLox. If no file names are specified",
    "    and the REPL was not explicitly run, it will be automatically",
    "    run after parsing is complete.",
    "",
    "Special Options:",
    "    -                   (with no short flags) run the REPL.",
    "    --                  The rest of the arguments are all filenames.",
    "",
    "Flags:",
    "    -?, -h, --help      produce this help message",
    "    -v, --verbose       turn up verbose operation flag",
    "    -n, --dryrun        turn up dryrun flag",
    "    -d, --debug         turn up debug level",
    "    -V, --no-verbose    turn down verbose operation flag",
    "    -N, --no-dryrun     turn down dryrun flag",
    "    -D, --no-debug      turn down debug level",
    "        --bist          run the built-in self test",
    "        --demo          run the quick demo code",
    "",
    "    Multiple short flags can be combined (so -dv will turn up both the",
    "    debug and verbose levels).",
    "",
    0
};

/** Respond to the --help command line option.
 *
 * Copy the contents of the help_text array to the standard
 * error channel; add a newline to each element of the array,
 * and stop at the trailing NULL pointer.
 */
static void
help ()
{
    const char **cursor = help_text;
    const char *line;

    while (NULL != (line = *cursor++)) {
        printf ("%s\n", line);
    }
    exit (EX_OK);
}

/** Parse command line argument.
 *
 * Step through the command line options, updating the
 * options state, and calling back when appropriate.
 *
 * @param argc number of strings in argv
 * @param argv array of argument strings
 *
 * By convention, argv[0] is the name of the program that is running,
 * and arguments parsed from the command line are placed in subsequent
 * entries. argv[argc] is set to NULL. Accessing elements beyond that
 * results in undefined behavior.
 */
void
parse_options (int argc, const char **argv)
{

    bool autoRepl = true;
    bool parseFlags = true;

    int argi = 1;

    while (argi < argc) {
        const char *argp = argv[argi++];

        if (parseFlags && (argp[0] == '-')) {
            if (argp[1] == '-') {

                if (argp[2] == '\0') {
                    parseFlags = false;
                    continue;
                }

                if (!strcmp ("help", argp + 2)) {
                    help ();
                    continue;
                }

                if (!strcmp ("verbose", argp + 2)) {
                    options.verbose++;
                    continue;
                }

                if (!strcmp ("debug", argp + 2)) {
                    options.debug++;
                    continue;
                }

                if (!strcmp ("dryrun", argp + 2)) {
                    options.debug++;
                    continue;
                }

                if (!strcmp ("no-verbose", argp + 2)) {
                    options.verbose--;
                    continue;
                }

                if (!strcmp ("no-debug", argp + 2)) {
                    options.debug--;
                    continue;
                }

                if (!strcmp ("no-dryrun", argp + 2)) {
                    options.debug--;
                    continue;
                }

                if (!strcmp ("bist", argp + 2)) {
                    INVAR (options.bist, "options: no bist function pointer.");
                    options.bist ();
                    continue;
                }

                if (!strcmp ("demo", argp + 2)) {
                    INVAR (options.demo, "options: no demo function pointer.");
                    options.demo ();
                    continue;
                }

            } else {

                if (argp[1] == '\0') {
                    autoRepl = false;
                    INVAR (options.repl, "options: no repl function pointer.");
                    options.repl ();
                } else {

                    char flag;

                    while ('\0' != (flag = *++argp)) {
                        switch (flag) {
                        case 'd':
                            options.debug++;
                            break;
                        case 'v':
                            options.verbose++;
                            break;
                        case 'n':
                            options.dryrun++;
                            break;
                        case 'D':
                            options.debug--;
                            break;
                        case 'V':
                            options.verbose--;
                            break;
                        case 'N':
                            options.dryrun--;
                            break;
                        case '?':
                            help ();
                            break;
                        case 'h':
                            help ();
                            break;
                        }
                    }
                }
            }
        } else {
            autoRepl = false;
            INVAR (options.cfar, "options: no file function pointer.");
            options.cfar (argp);
        }

        if (autoRepl) {
            INVAR (options.repl, "options: no repl function pointer.");
            options.repl ();
        }
    }
}
