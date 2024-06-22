#pragma once

#include "bist.h"
#include "common.h"
#include "post.h"

// In Lox, all (non-string) values are floating point.

typedef double Value;

post_fn post_value;
bist_fn bist_value;
