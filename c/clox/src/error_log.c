#include "error_log.h"

#include <stdio.h>

/** @file error_log.c
 * @brief Support code for error_log macros.
 */

/** Print a Message with File and Line
 *
 * This is the utility function that organizes the output
 * in the form used by compiler errors and warnings, which
 * allows the editor to seek directly to the proper file
 * and line.
 *
 * Before actually writing to standard error, a newline is
 * sent to standard output, then it is flushed. This assures
 * that our error message starts at the left margin, after
 * assuring all prior output is made visible.
 *
 * @param file a path to a source file (or similar).
 * @param line the line number in the source file (or similar).
 * @param func the containing function name (or similar).
 * @param pfx a "prefix" string to print on the first line
 * @param cond the "condition" that failed
 * @param msg an extended message to follow
 *
 * Any or all parameters can be NULL, "", or zero; in each case,
 * such parameters will be skipped along with connective text.
 *
 * The message ends with a blank line.
 */
void
_error_log (
    const char *file,
    int line,
    const char *func,
    const char *pfx,
    const char *cond,
    const char *msg)
{
    printf ("\n");
    fflush (stdout);

    if (file && file[0])
        if (line)
            fprintf (stderr, "%s:%d: ", file, line);
        else
            fprintf (stderr, "%s: ", file);
    else if (line)
        fprintf (stderr, "Line %d: ", line);

    if (pfx && pfx[0])
        fprintf (stderr, "%s ", pfx);

    if (func && func[0])
        fprintf (stderr, "in %s\n", func);

    if (cond && cond[0])
        fprintf (stderr, "    %s\n", cond);

    if (msg && msg[0])
        fprintf (stderr, "%s\n", msg);

    fprintf (stderr, "\n");
}
