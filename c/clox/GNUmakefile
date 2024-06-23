# -*- Makefile -*-


# This C project builds a program called "clox"
MAIN            := clox

# I will be telling GCC to constrain us to C99
# (formal title, INCITS ISO/IEC 9899:1999)
# until original author deviates significantly.

CC		:= c99

# NOTE: this is GCC-specific.
COPTS		:= -fshort-enums

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

# tests: run power-on and built-in self tests.
#
# NOTE: a nonzero exit status from BIST is now treated
# as a build failure.

tests::		$(BIND)$(MAIN)
	$P '  %-14s\n' "POST and BIST"
	$Q $(BIND)$(MAIN) bist

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

format::	$(EXES)
	$C ${BIND}indent-c.sh $(SRCD)*.c
	$C ${BIND}indent-h.sh $(INCD)*.h

# The presence of "STUB ();" calls is not a build error, but expose
# where they all are to point out what needs to be implemented next.
stubs::
	$P 'Looking for stubs in %s*.c and %s*.c ...\n' '$(SRCD)' '$(INCD)'
	$C grep -n 'STUB ();' $(SRCD)*.c $(INCD)*.h /dev/null || true
