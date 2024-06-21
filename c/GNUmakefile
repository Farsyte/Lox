# -*- Makefile -*-

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

# Set up conventional directories

BIND            := bin/
DEPD            := dep/
INCD            := inc/
OBJD            := obj/
SRCD            := src/

# Set up conventional name lists

SRCS            := $(wildcard $(SRCD)*.c)
OBJS            := $(SRCS:$(SRCD)%.c=$(OBJD)%.o)
DEPS            := $(SRCS:$(SRCD)%.c=$(DEPD)%.d)
EXES            := $(MAIN:%=$(BIND)%)

ifeq ($(C_DBG_OPT),)
C_DBG_OPT	:= -g -O2
endif

ifeq ($(C_WARNINGS),)
C_WARNINGS	:= -W -Wall -Wextra -Werror
endif

# Pick compiler and set up command line options
ifeq ($(CC),)
CC              := gcc
endif

ifeq ($(CFLAGS),)
CFLAGS          := $(C_DBG_OPT) $(C_WARNINGS) $(COPTS) -I$(INCD)
endif


# The default build is "make all"
default::       all

# make all: assure all executables are up-to-date.

all::           $(EXES)

# Each time we build a .o file, we also record the current
# set of source and header dependencies in a .d file. That way,
# if a source or header change is made, all .o files that used
# it in their previous compile will be recompiled.
#
# Side note: if $(DEPD)%.d had dependencies for %.o, then printing $^
# will print the headers it depends on. This would get cumbersome, so
# flip to just printing $* (the basename of the file).

$(OBJD)%.o:     $(SRCD)%.c
	$C [ -d $(OBJD) ] || mkdir -p $(OBJD)
	$C [ -d $(DEPD) ] || mkdir -p $(DEPD)
	$P '  %-6s %s\n' "CC" "$*"
	$Q $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $< -MM | \
	 sed '1s|^$*\.o: |$@: |' > ${DEPD}$*.d
	$Q $(COMPILE.c) $(OUTPUT_OPTION) $<

# This includes all of the dependencies we have recorded.
# The trailing /dev/null is needed in case there are no .d files.

include $(wildcard $(DEPD)*.d) /dev/null

# "make clean" removes whatever is built by the build.

clean::
ifneq ($(EXES),)
	$X $(EXES)
	$C [ ! -d $(BIND) ] || $Y $(BIND)
endif
ifneq ($(DEPS),)
	$X $(DEPS)
	$C [ ! -d $(DEPD) ] || $Y $(DEPD)
endif
ifneq ($(OBJS),)
	$X $(OBJS)
	$C [ ! -d $(OBJD) ] || $Y $(OBJD)
endif

# If no MAIN was specified, drop all the rules relating to
# linking and running the program. The directory is probably
# just building a library.
#
# No support (yet) for multiple C programs in one directory.
# I prefer to refactor that into a directory containing a
# library of shared code, plus a directory for each program.

ifneq ($(MAIN),)

$(BIND)$(MAIN): $(OBJS)
	$C [ -d $(BIND) ] || mkdir -p $(BIND)
	$P '  %-6s %s\n' "LD" "$@"
	$Q $(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

# If the caller sets ARGS, it has arguments to hand to $(MAIN)
# If the caller sets FROM, redirect the input from that file.
# If the caller sets INTO, redirect the output into that file.

ifneq ($(INTO),)
RED_OUT         := > $(INTO)
clean::         ; $X $(INTO)
else
RED_OUT         :=
endif

ifneq ($(FROM),)
RED_IN          := < $(FROM)
clean::         ; $X $(FROM)
else
RED_IN          :=
endif

run:: $(BIND)$(MAIN)
	$P '  %-14s %s\n' "RUN C" "$(MAIN)"
	$(BIND)$(MAIN) $(ARGS) $(RED_OUT) $(RED_IN)


endif

# Make sure we have a do-nothing "run" target even if MAIN is not set.
run::                                                           ; $T
.PHONY: run
