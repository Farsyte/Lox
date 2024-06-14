# -*- Makefile -*-

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

TID		:= tid/
TOD		:= tod/

default all clean::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak $@

# CAREFUL: use "make run" only in interactive environments.
# In particular, running this in an Emacs compile buffer will
# cause the build to wait for input that never arrives.
run::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak $@

# Before running tests, flush prior results.
# After tests are complete:
# - use "git status" to detect missing or added output files
# - use "git diff" to detect changes in test outputs

tests::
	$X $(TOD)*.out
	$C [ ! -d $(TOD) ] || $Y $(TOD)
	$C mkdir $(TOD)

# Parameter Parsing Tests
# testing requires changing how the program is run.

tests::
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-1.out -f ../java.mak  run ARGS='hello.lox'
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-2.out -f ../java.mak irun ARGS='-v hello.lox'

clean::
	$X $(TOD)args-1.out
	$X $(TOD)args-2.out

# REPL testing: run the program with no parameters,
# change the input file from test to test.

RTIF            := $(wildcard $(TID)repl*.in)

ifneq ($(RTIF),)

RTOF            := $(RTIF:$(TID)repl%.in=$(TOD)repl%.out)

tests::		$(RTOF)

$(TOD)repl%.out:	$(TID)repl%.in
	$M PACKAGE=lox MAIN=Lox FROM=$< INTO=$@ -f ../java.mak run

clean::
	$X $(RTOF)

endif

clean::
	$C [ ! -d $(TOD) ] || $Y $(TOD)
