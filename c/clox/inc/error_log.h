#pragma once

#include <stdlib.h>                     /* for abort() */
#include <sysexits.h>

/** @file error_log.h
 * @brief Error Logging Macros
 *
 * This file defines several macros that can be used to emit
 * messages to the standard error stream that contain source
 * file names and line numbers.
 */

/** Indicate we reached unimplemented code.
 *
 * This macro emits a message via ERROR_LOG indicating that we
 * have reached a stub in the source code, then terminates the
 * program normally.
 */
#define STUB(msg)                               \
    do {                                        \
        ERROR_LOG("STUB Encountered", 0, msg);  \
        exit(EX_OK);                            \
    } while (0)

/** Abort if an invariant is violated.
 *
 * This macro checks that a condition is met. If not, ERROR_LOG
 * is used to report the failed condition. The program is then
 * aborted in a way that will dump a core file (if enabled) or
 * drop to a command line in a debugger (if in use).
 */
#define INVAR(cond, msg)                                \
    do {                                                \
        if (cond) break;                                \
        ERROR_LOG("Condition Violated", #cond, msg);    \
        abort();                                        \
    } while (0)

/** Log an error with source file name and line.
 *
 * This macro formats and emits a message to the standard error
 * stream identifying the source location and including the
 * provided Prefix, Condition, and Message strings:
 *
 *      <<path/to/filename.ext>>:<<line>>: <<pfx> in <<func>>
 *          <<cond>>
 *      <<msg>>
 *
 * @param pfx a "prefix" string to print on the first line
 * @param cond the "condition" that failed
 * @param msg an extended message to follow
 *
 * Any or all of the parameters can be NULL or "" in which
 * case that message and some surrounding connective space
 * will be skipped.
 */
#define ERROR_LOG(pfx, cond, msg)               \
    do {                                        \
        _error_log(__FILE__, __LINE__,          \
                   __func__,                    \
                   pfx, cond, msg);             \
    } while (0)

extern void _error_log (const char *file, int line, const char *func, const char *pfx, const char *condstr, const char *msg);
