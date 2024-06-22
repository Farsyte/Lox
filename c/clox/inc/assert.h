#pragma once

#include <stdlib.h>             /* for abort() */

#define assert(cond, msg)			\
    do {					\
	if (cond) break;			\
	build_msg(				\
	    "Assert Failed: ", #cond, msg);	\
	abort();				\
    } while (0)

#define build_msg(pfx, cond, msg)		\
    do {					\
	_build_msg(__FILE__, __LINE__,		\
		   __func__,			\
		   pfx, cond, msg);		\
    } while (0)

/** INTERNAL: print a message with file and line
 *
 * This is the utility function that organizes the output
 * in the form used by compiler errors and warnings, which
 * allows the editor to seek directly to the proper file
 * and line.
 */
int _build_msg (
    const char *file,
    int line,
    const char *func,
    const char *pfx,
    const char *condstr,
    const char *msg);
