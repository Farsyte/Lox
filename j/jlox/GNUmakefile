# -*- Makefile -*-

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

default all clean::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak $@

# CAREFUL: use "make run" only in interactive environments.
# In particular, running this in an Emacs compile buffer will
# cause the build to wait for input that never arrives.
run::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak $@

hello::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak run ARGS='hello.lox'

hello-v::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak irun ARGS='-v hello.lox'
