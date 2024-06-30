#pragma once

#include "types.h"

struct options {
    int verbose;
    int dryrun;
    int debug;
    void (
        *bist) (
        );
    void (
        *demo) (
        );
    void (
        *cfar) (
        const char *);
    void (
        *repl) (
        );
};

extern Options options;

extern void parse_options (
    int argc,
    const char **argv);
