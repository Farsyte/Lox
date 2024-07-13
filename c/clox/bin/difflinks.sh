#!/bin/bash -
#
# set up links in c/clox allowing emacs to find
# the targets of error messages that are noted
# as added to or removed from a file with diff.

set -euo pipefail
set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
clox=$(dirname "$dp")

export LC_COLLATE="C"

rm -f "$clox/+src"
rm -f "$clox/-src"
rm -f "$clox/+inc"
rm -f "$clox/-inc"

ln -s src "$clox/+src"
ln -s src "$clox/-src"
ln -s inc "$clox/+inc"
ln -s inc "$clox/-inc"
