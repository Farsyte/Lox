# -*- Makefile -*-

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

TID             := tid/
TOD             := tod/

# make: the default build is to build the interpreter

default::       build

# make all: build the interpreter and run the tests (see below)

all::           build

# make all: use ../java.mak to build each package.

build::
	$M PACKAGE=lox -f ../java.mak $@

# make clean: use ../java.mak to clean each package.

clean::
	$M PACKAGE=lox -f ../java.mak $@

# make run: run Lox
# Assure that the "lox" package has been built, then run
# the main function in the Lox class.
#
# CAREFUL: use "make run" only in interactive environments.
# In particular, running this in an Emacs compile buffer will
# cause the build to wait for input that never arrives.
#
# This starts an interactive Lox interpreter, or whatever
# parts of the interpreter have been implemented so far.

run::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak $@


# Before running tests, flush prior results.
# After tests are complete:
# - use "git status" to detect missing or added output files
# - use "git diff" to detect changes in test outputs

tests::		build
	$X $(TOD)*.out
	$C [ ! -d $(TOD) ] || $Y $(TOD)
	$C mkdir $(TOD)
	$M eachtest

# include the tests in "make world"
# - weakness: "make tests" rechecks "make build"

world::
	$M tests

#
# Parameter Parsing Tests
# testing requires changing how the program is run.

# Note that parsing "hello.lox" starts throwing failures
# when the scanner is being implemented, and will continue
# to fail until much later in the implementation process.

ARGSTESTS       := $(TOD)args-1.out $(TOD)args-2.out
eachtest::      $(ARGSTESTS)

$(TOD)args-1.out:
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-1.out -f ../java.mak irun ARGS='hello.lox'

$(TOD)args-2.out:
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-2.out -f ../java.mak irun ARGS='-v hello.lox'

clean::
	$X $(ARGSTESTS)

# REPL testing: run the program with no parameters,
# change the input file from test to test.

RTIF            := $(wildcard $(TID)repl*.in)

ifneq ($(RTIF),)

RTOF            := $(RTIF:$(TID)repl%.in=$(TOD)repl%.out)

eachtest::      $(RTOF)

$(TOD)repl%.out:        $(TID)repl%.in
	$M PACKAGE=lox MAIN=Lox FROM=$< INTO=$@ -f ../java.mak run

clean::
	$X $(RTOF)

endif

clean::
	$C [ ! -d $(TOD) ] || $Y $(TOD)

# PROG testing: run the program with no parameters,
# change the input file from test to test.

PTIF            := $(wildcard $(TID)prog*.lox)

ifneq ($(PTIF),)

RTOF            := $(PTIF:$(TID)prog%.lox=$(TOD)prog%.out)

eachtest::      $(RTOF)

$(TOD)prog%.out:        $(TID)prog%.lox
	$P '  %-14s %s\n' "Lox" "$< > $@"
	$M PACKAGE=lox MAIN=Lox INTO=$@ -f ../java.mak run $< ARGS="$<"

clean::
	$X $(RTOF)

endif

clean::
	$C [ ! -d $(TOD) ] || $Y $(TOD)

