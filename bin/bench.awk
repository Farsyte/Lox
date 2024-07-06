#!/usr/bin/awk

BEGIN {
    p = 0;
}

/elapsed time:$/ {
    p = 2;
}

p>0 {
    print;
    p--;
}
