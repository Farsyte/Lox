# -*- Makefile -*-

# Adjust verbosity of the output

Q               := @
I               := $Q -
C               := $Q

# Command shortcuts

X               := $C /bin/rm -f
Y               := /bin/rmdir --ignore-fail-on-non-empty -p
T               := $C true
F               := $C false
E               := $C echo
P               := $C printf
M               := $C $(MAKE)

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

.PHONY:                 default all clean world stubs $(SUBD) $(SUBA) $(SUBC)

