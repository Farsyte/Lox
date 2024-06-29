# -*- Makefile -*-

# Adjust verbosity of the output

# $Q uses "@" to tell Make to not echo the command.
# If you say "make Q= ..." this is disabled, and
# commands are printed before running them.
#
# This is typically used, directly, for commands that,
# if they fail, should fail the build.
#
# For example, running the C compiler.

Q               := @

# $I uses "-" to tell Make to ignore nonzero exit
# status from this command.
# If you say "make I= ..." this is disabled, and
# commands that we usually allow to "fail" will now
# terminate the build.
#
# This is typically used, directly, for commands that are always
# echoed to the output when run, but which do not terminate the build
# if they have nonzero exit status.

I               := -

# $C combines $Q and $I: it says "this is a command
# that we want to run in the normal way, and normally
# without printing it and ignoring any nonzero exit
# status."
#
# This is used for commands that are usually not printed
# when run, and whose nonzero exit status does not cause
# the build to end.

C               := $Q $I

# Command shortcuts

# $X silently removes the listed files if they exist,
# ignoring things that do not exist and things that
# are not files.

X               := $C /bin/rm -f

# $Y silently removes the listed directories if they
# exist and are empty, then repeats that for parents
# until a non-empty parent is found.

Y               := /bin/rmdir --ignore-fail-on-non-empty -p

# $T is the easy "this always succeeds" command.

T               := $Q true

# $T is the easy "this always fails" command.

F               := $Q false

# $E echos the text as per "echo" rules. Unless you have
# provided flags otherwise, it will include a newline.

E               := $C echo

# $P uses the first agument to format the values of the rest
# of the arguments as per "printf" rules. You probably want
# to remember to have a \n at the end.

P               := $C printf

# The "+" here allows us to use "-j <number>" to run at most <number>
# parallel build jobs. It has the same effect as having $(MAKE) at the
# start of the line.

M               := + $C $(MAKE)

# Locate the top of the current Git repo, if not already set.
ifeq ($(TOP),)
export TOP      := $(shell git rev-parse --show-toplevel)/
endif

# Locate subdirectories for recursive build

SUBMAKES        := $(wildcard */GNUmakefile)
SUBDIRS         := $(dir $(SUBMAKES))

SUBA            := $(addsuffix .all,$(SUBDIRS))
SUBC            := $(addsuffix .clean,$(SUBDIRS))
SUBD            := $(addsuffix .default,$(SUBDIRS))

# Common Build Targets

default::               $(SUBD)                                 ; @true
all::                   $(SUBA)                                 ; @true
clean::                 $(SUBC)                                 ; @true
world::
	$M clean
	$M all

$(SUBD):
	$M -C "$(@:%.default=%)"
$(SUBA):
	$M -C "$(@:%.all=%)" all
$(SUBC):
	$M -C "$(@:%.clean=%)" clean

.PHONY:         default all clean world stubs $(SUBD) $(SUBA) $(SUBC) sync

# Run this target after changing branches

sync::
	$M -k -j clean
	$M -k -j -C j/jlox tests
	$M -C c/clox cycle


