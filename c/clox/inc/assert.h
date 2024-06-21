#pragma once

#define assert(cond, msg)				\
    do {						\
	(void)((cond) || _assert(			\
		   __FILE__, __LINE__,			\
		   __PRETTY_FUNCTION__, #cond, msg));	\
    } while (0)

int _assert (
    const char *file,
    int line,
    const char *func,
    const char *condstr,
    const char *msg);
