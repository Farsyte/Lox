# -*- Makefile -*-

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

default all clean run::
	$M PACKAGE=hello MAIN=Hello -f ../java.mak $@
