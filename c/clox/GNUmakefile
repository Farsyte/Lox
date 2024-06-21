# -*- Makefile -*-


# This C project builds a program called "clox"
MAIN            := clox

# I will be telling GCC to constrain us to C99
# (formal title, INCITS ISO/IEC 9899:1999)
# until original author deviates significantly.

CC		:= c99

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))


# indent: process all source files with indent.
#
# This should ONLY be done after a successful build, because "indent"
# works best if the input has no syntax errors.
#
# This should ALWAYS be done in a Git branch as part of the last
# commit before merging the branch back into the trunk.

indent::
	$P '  %-14s %s\n' "INDENT" "$(SRCD)*.c $(INCD)*.h"
	$Q indent $(SRCD)*.c $(INCD)*.h
