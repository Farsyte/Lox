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
        # TODO: pull @file block comments up to here.
        echo
        (
            cd "$top"
            hn=''
            sep=''
            for fnp in $(echo $b | tr '_' ' ')
            do
                hn="${hn}${sep}${fnp}"
                find inc -iname "$hn.h" -print |
                    sed 's:^inc/:#include ":' |
                    sed 's:$:":'
                sep='_'
            done | tail -1 || true
            echo
            (grep '#include "' < $c || true) | sort
            echo
            (grep '#include <' < $c || true) | sort
            echo
        ) | awk -f "$top/bin/uniq.awk"
        echo
        grep -v '#include' < $c || true
    ) | cat -s | sed '1d' | indent -st | expand > $o

    if cmp --silent $c $o
    then
        rm -f "$o"
    else
        printf '  %-6s %s\n' "INDENT" "$c"
        mv "$c" "$c"~
        mv "$o" "$c"
    fi
done

rmdir --ignore-fail-on-non-empty --parents "$td" >& /dev/null
