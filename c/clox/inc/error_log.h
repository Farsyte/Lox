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

/** Indicate we reached "unreachable" code.
 *
 * This macro emits a message via ERROR_LOG indicating that we have
 * reaced a point in the source code that should be unreachable,
 * usually via an enum variable containing a value not in the
 * enumeration, then terminates the program normally.
 */
#define UNREACHABLE(msg)                                \
    do {                                                \
        ERROR_LOG("Reached Unreachable Code!", 0, msg); \
        abort();                                        \
    } while (0)

/** Abort if an invariant is violated.
 *
 * This macro checks that a condition is met. If not, ERROR_LOG
 * is used to report the failed condition. The program is then
 * aborted in a way that will dump a core file (if enabled) or
 * drop to a command line in a debugger (if in use).
 */
#define INVAR(cond, ...)                                \
    do {                                                \
        if (cond) break;                                \
        ERROR_LOG("Condition Violated", #cond,          \
                  __VA_ARGS__);                         \
        abort();                                        \
    } while (0)

/** Log to stderr with source file name and line.
 *
 * This macro formats and emits a message to the standard error
 * stream identifying the source location and including the
 * provided Prefix, Condition, and Message strings:
 *
 *      <<path/to/filename.ext>>:<<line>>: <<pfx> in <<func>>
 *          <<cond>>
 *      <<printf-style-formatted-message>>
 *
 * @param pfx a "prefix" string to print on the first line
 * @param cond the "condition" that failed
 * Parameters after "cond" are a printf-like definition of
 * the message to print.
 *
 * Any or all of the parameters can be NULL or "" in which
 * case that message and some surrounding connective space
 * will be skipped.
 */
#define ERROR_LOG(pfx, cond, ...)               \
    do {                                        \
        _error_log(__FILE__, __LINE__,          \
                   __func__,                    \
                   pfx, cond, __VA_ARGS__);     \
    } while (0)

/** Log to stdout with source file name and (maybe) line.
 *
 * This macro formats and emits a message to the standard error
 * stream identifying the source location and including the
 * provided Prefix, Condition, and Message strings:
 *
 *      <<path/to/filename.ext>>:<<line>>: <<pfx> in <<func>>
 *          <<cond>>
 *      <<printf-style-formatted-message>>
 *
 * @param pfx a "prefix" string to print on the first line
 * @param cond the "condition" that failed
 * Parameters after "cond" are a printf-like definition of
 * the message to print.
 *
 * Any or all of the parameters can be NULL or "" in which
 * case that message and some surrounding connective space
 * will be skipped.
 */
#define DEBUG_LOG(pfx, cond, ...)               \
    do {                                        \
        _debug_log(__FILE__, __LINE__,          \
                   __func__,                    \
                   pfx, cond, __VA_ARGS);       \
    } while (0)

/** Log to stdout (compact)with source file name and (maybe) line.
 *
 * This macro formats and emits a message to the standard error
 * stream identifying the source location and including the
 * provided Prefix, Condition, and Message strings:
 *
 *      function: message...
 *
 * The entire list of parameters for this macro are
 * a printf-like message definition.
 *
 * Any or all of the parameters can be NULL or "" in which
 * case that message and some surrounding connective space
 * will be skipped.
 */
#define DEBUG_LINE(...)                         \
        _debug_line(__func__, __VA_ARGS__);

extern void _error_log (const char *file, int line, const char *func, const char *pfx, const char *condstr, const char *msg, ...);
extern void _debug_log (const char *file, int line, const char *func, const char *pfx, const char *condstr, const char *msg, ...);
extern void _debug_line (const char *func, const char *format, ...);
