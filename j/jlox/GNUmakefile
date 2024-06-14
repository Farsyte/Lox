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

tests::
	$C [ -d $(TOD) ] || mkdir -p $(TOD)
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-1.out -f ../java.mak  run ARGS='hello.lox'
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-2.out -f ../java.mak irun ARGS='-v hello.lox'

clean::
	$X $(TOD)args-1.out
	$X $(TOD)args-2.out

RTIF            := $(wildcard $(TID)repl*.in)

ifneq ($(RTIF),)

RTOF            := $(RTIF:$(TID)repl%.in=$(TOD)repl%.out)

tests::
	$X $(RTOF)
	$M $(RTOF)

$(TOD)repl%.out:	$(TID)repl%.in
	$C [ -d $(TOD) ] || mkdir -p $(TOD)
	$M PACKAGE=lox MAIN=Lox FROM=$< INTO=$@ -f ../java.mak run

clean::
	$X $(RTOF)

endif
