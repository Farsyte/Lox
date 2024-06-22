#!/bin/bash -
set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")
td="$top/tmp"
tmp="$td/.cfix"

export LC_COLLATE="C"

for c in "$@"
do
    [ -r "$c" ] || continue

    d=$(dirname "$c")
    b=$(basename "$c" .c)
    o="$tmp.$b.c"

    [ -d "$td" ] || mkdir -p "$td"
    (
        echo
        (
            cd "$top"
            find inc -iname "$b.h" -print |
                sed 's:^inc/:#include ":' |
                sed 's:$:":'
            echo
            (grep '#include "' < $c || true) | sort
            echo
            (grep '#include <' < $c || true) | sort
            echo
        ) | awk -f "$top/bin/uniq.awk"
        echo
        grep -v '#include' < $c || true
    ) | cat -s | sed '1d' | indent -st > $o

    if cmp --silent $c $o
    then
        rm -f "$o"
    else
        printf '  %-14s %s\n' "INDENT" "$c"
        mv "$c" "$c"~
        mv "$o" "$c"
    fi
done

rmdir --ignore-fail-on-non-empty --parents "$td" >& /dev/null
