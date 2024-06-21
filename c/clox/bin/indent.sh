#!/bin/bash -
#
# Indent a list of files.

rp=$(realpath "$0")
dp=$(dirname "$rp")
top=$(dirname "$dp")
td="$top/tmp"
tmp="$td/.indented"

for f in "$@"
do
    [ -r "$f" ] || continue

    b=$(basename "$f")
    o="$tmp.$b"
    [ -d "$td" ] || mkdir -p "$td"
    indent -st < "$f" > "$o"
    if cmp --silent "$f" "$o"
    then
        rm -f "$o"
    else
        mv "$f" "$f"~
        mv "$o" "$f"
    fi
done

rmdir --ignore-fail-on-non-empty --parents "$td" >& /dev/null
