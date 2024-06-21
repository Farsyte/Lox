#pragma once

#include "assert.h"             /* for build_msg */

#define POST_PASS	0
#define POST_FAIL	1

#define POST_CHECK(cond, msg)			\
    do {					\
	if (cond) break;			\
	build_msg("POST failed", #cond, msg);	\
	exit_code |= POST_FAIL;			\
    } while (0)

int post_all (
    );
