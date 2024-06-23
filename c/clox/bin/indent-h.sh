#!/bin/bash -
set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")
td="$top/tmp"
tmp="$td/.hfix"

export LC_COLLATE="C"

for h in "$@"
do
    [ -r "$h" ] || continue

    d=$(dirname "$h")
    b=$(basename "$h")
    o="$tmp.$b.o"

    [ -d "$td" ] || mkdir -p "$td"

    (
        echo '#pragma once'
        echo
        (grep '#include "' < $h || true) | sort
        echo
        (grep '#include <' < $h || true) | sort
        echo
        (grep -v '#pragma once' < $h || true) | (grep -v '#include' || true)
    ) | cat -s | indent -st >> $o

    if cmp --silent $h $o
    then
        rm -f "$o"
    else
        printf '  %-14s %s\n' "INDENT" "$h"
        mv "$h" "$h"~
        mv "$o" "$h"
    fi
done

rmdir --ignore-fail-on-non-empty --parents "$td" >& /dev/null
