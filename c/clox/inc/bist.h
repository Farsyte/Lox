#pragma once

#define BIST_PASS	0
#define BIST_FAIL	1

#define BIST_CHECK(cond, msg)			\
    do {					\
	if (cond) break;			\
	build_msg("BIST failed", #cond, msg);	\
	exit_code |= BIST_FAIL;			\
    } while (0)

int bist_all (
    int argc,
    const char *argv[]);
