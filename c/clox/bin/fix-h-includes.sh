#!/bin/bash -

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")
td="$top/tmp"
tmp="$td/.hfix"

for h in "$@"
do
    [ -r "$h" ] || continue

    d=$(dirname "$h")
    b=$(basename "$h")
    o="$tmp.$b.c"

    [ -d "$td" ] || mkdir -p "$td"
    (
        echo '#pragma once' > $o
        echo
        grep '#include "' < $h | sort
        echo
        grep '#include <' < $h | sort
        echo
        grep -v '#pragma once' < $h | grep -v '#include'
    ) | cat -s >> $o

    if cmp --silent $h $o
    then
        rm -f "$o"
    else
        mv "$h" "$h"~
        mv "$o" "$h"
    fi
done

rmdir --ignore-fail-on-non-empty --parents "$td" >& /dev/null
