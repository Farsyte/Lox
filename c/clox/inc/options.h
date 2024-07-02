#pragma once

#include "types.h"

/** Command Line Options structure layout.
 */
struct options_s {
    int verbose;                ///< verbosity, 0=default, higher is more verbose.
    int dryrun;                 ///< dryrun level, 0=default, higher is skipping more stuff.
    int debug;                  ///< debug level, 0=default, higher is more debugging.

    void (*bist) ();            ///< Callback for Built-In Self Test
    void (*demo) ();            ///< Callback for Demonstration Mode
    void (*repl) ();            ///< Callback to run REPL

    /** Callback for file names
     * @param path name of file
     */
    void (*cfar) (const char *path);
};

extern void parse_options (int argc, const char **argv);
