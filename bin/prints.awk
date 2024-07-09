#!/usr/bin/awk -f

BEGIN {
    p = 0;
    e = 0
}

/Executing ...$/ {
    e = 1;
    next;
}

/Executing ... done./ {
    e = 0;
    next;
}

/OP_PRINT/ {
    if (e) p = 1;
    next;
}

p>0 {
    print;
    p--;
}
