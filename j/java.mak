# -*- Makefile -*-

# Inherit recurive build behaviors
include ../GNUmakefile

# Rules for building a specific java package.
#
# The GNUmakefile in a subproject will use this file
# separately for each package it contains.
#
# usage:
#    make PACKAGE=lox -f ../java.mak target [target ...]

# Set up conventional directories

SRCD            := src/
CLSD            := cls/

PTOP            := farsyte

PKGD            := $(PTOP)/$(PACKAGE)/

# Set up conventional name lists

JSRC            := $(wildcard $(SRCD)$(PKGD)*.java)
JCLS            := $(JSRC:$(SRCD)$(PKGD)%.java=$(CLSD)$(PKGD)%.class)

# Pick compiler and set up command line options

JC              := javac
JCFLAGS         := -cp $(SRCD) -d $(CLSD) -g -Werror -implicit:none

JR              := java
JRFLAGS         := -cp $(CLSD) -ea -esa

default::       build
all::           build
build::         $(JCLS)
clean::
	$X $(JCLS)
	$C [ ! -d $(CLSD)$(PKGD) ] || $Y $(CLSD)$(PKGD)

$(CLSD)$(PKGD)%.class:  $(SRCD)$(PKGD)%.java
	$P '  %-14s %s\n' "JAVAC" "$*"
	$C $(JC) $(JCFLAGS) $<

# If the caller sets MAIN=Cls, then add ruiles to
# run the main function from the $(PTOP).$(PACKAGE).$(MAIN) class.
#
# If the caller sets FROM, redirect the input from that file.
#
# If the caller sets INTO, redirect the output into that file.

ifneq ($(MAIN),)

clean::
	$X $(FROM) $(INTO)

ifneq ($(INTO),)
RED_OUT         := > $(INTO) 2>&1
else
RED_OUT         := 
endif

ifneq ($(TEE),)
RED_TEE         := |& tee $(TEE)
else
RED_TEE         := 
endif

ifneq ($(FROM),)
RED_IN          := < $(FROM)
else
RED_IN          := 
endif

run:: $(JCLS)
	$P '  %-14s %s\n' "RUN JAVA" "$(MAIN) $(RED_IN) $(RED_OUT) $(RED_TEE)"
	$C $(JR) $(JRFLAGS) $(PTOP).$(PACKAGE).$(MAIN) $(ARGS) $(RED_IN) $(RED_OUT) $(RED_TEE)

irun:: $(JCLS)
	$P '  %-14s %s\n' "RUN JAVA" "$(MAIN) $(RED_IN) $(RED_OUT) $(RED_TEE)"
	$I $(JR) $(JRFLAGS) $(PTOP).$(PACKAGE).$(MAIN) $(ARGS) $(RED_IN) $(RED_OUT) $(RED_TEE)

.PHONY: run

endif
