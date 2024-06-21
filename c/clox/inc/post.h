#pragma once

#include "assert.h"

#define POST_PASS	0
#define POST_FAIL	1

#define POST_CHECK(cond, msg)			\
    do {					\
	if (cond) break;			\
	build_msg("POST failed", #cond, msg);	\
	exit_code |= POST_FAIL;			\
    } while (0)

#define POST_CHECK_EQ_uint(expected, observed, msg)			\
    do {								\
	unsigned long long exp = (unsigned long long)(expected);	\
	unsigned long long obs = (unsigned long long)(observed);	\
	if (exp == obs) break;						\
	build_msg("POST failed", #expected " == " #observed, msg);	\
	fprintf(stderr, "  expected: %llu\n", exp);			\
	fprintf(stderr, "  observed: %llu\n", obs);			\
	exit_code |= POST_FAIL;						\
    } while (0)

int post_all (
    );
