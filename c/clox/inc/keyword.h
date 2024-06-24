#pragma once

#include "bist.h"
#include "post.h"
#include "token_type.h"

TokenType keyword_token_type (
    const char *start,
    int len);

bist_fn bist_keyword;
post_fn post_keyword;
