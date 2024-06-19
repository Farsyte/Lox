# -*- Makefile -*-

# This C project builds a program called "fib"
MAIN            := fib

# Compile fib with best optimization and no debug support.
C_DBG_OPT	:= -O3

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

