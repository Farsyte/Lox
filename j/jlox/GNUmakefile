# -*- Makefile -*-

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

default all clean run::
	$M PACKAGE=lox MAIN=Lox -f ../java.mak $@


