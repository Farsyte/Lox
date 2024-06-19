# -*- Makefile -*-


# This C project builds a program called "clox"
MAIN            := clox

# I will be telling GCC to constrain us to C99
# (formal title, INCITS ISO/IEC 9899:1999)
# until original author deviates significantly.

CC		:= c99

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

