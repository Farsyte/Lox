# -*- Makefile -*-

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

TID             := tid/
TOD             := tod/

# make: the default build is to build the interpreter

default::       build

# make all: build the interpreter and run the tests (see below)

all::           build

# make all: use ../java.mak to build all buildables
# in the main package, AFTER checking that $(AstImplSrcs)
# containing the AST code (Expr.java) is ready.

build::         generate
	$M PACKAGE=lox  -f ../java.mak $@

# make clean: use ../java.mak to clean each package.
# Sendding MAIN=... to each line would tell it to clean
# up log files in $(OUTD), which this subproject is
# not currently using.

clean::
	$M PACKAGE=tool -f ../java.mak $@
	$M PACKAGE=lox  -f ../java.mak $@

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

run::           build
	$M PACKAGE=lox MAIN=Lox -f ../java.mak $@


# In this directory, "make world" includes the usual tests.

world::
	$M tests

# Before running tests, flush prior results.
# After tests are complete:
# - use "git status" to detect missing or added output files
# - use "git diff" to detect changes in test outputs

tests::         build
	$X $(TOD)*.out
	$C [ ! -d $(TOD) ] || $Y $(TOD)
	$C mkdir $(TOD)
	$M eachtest

all::           tests
#
# Parameter Parsing Tests
# testing requires changing how the program is run.

# Note that parsing "tid/hello.lox" starts throwing failures
# when the scanner is being implemented.

ARGSTESTS       := $(TOD)args-1.out $(TOD)args-2.out
eachtest::      $(ARGSTESTS)

$(TOD)args-1.out:
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-1.out -f ../java.mak irun ARGS='tid/hello.lox'

$(TOD)args-2.out:
	$M PACKAGE=lox MAIN=Lox FROM=/dev/null INTO=$(TOD)args-2.out -f ../java.mak irun ARGS='-v tid/hello.lox'

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

# The AST Implementation is currently auto-generated,
# and at the moment, there is only one (Expr). There
# will be another ... soon.
#
# Identify the files we intend to generate and the class
# to generate them. If the list is empty, skip over all
# the cruft related to them.

AstImplClasses  := Expr

ifneq ($(AstImplClasses),)

# Locate the generator source and verify it exists.
# Bail out if the package name or class name has been
# set to "", or if no source file is present.

AstGenPkg       := tool
ifneq ($(AstGenPkg),)

AstGenMain      := GenerateAst
ifneq ($(AstGenMain),)

AstGenSrc       := $(wildcard src/farsyte/$(AstGenPkg)/$(AstGenMain).java)
ifneq ($(AstGenSrc),)

AstImplDir      := src/farsyte/lox/
AstImplSrcs     := $(AstImplClasses:%=$(AstImplDir)%.java)

# We have generated sources and a generator.
# Assure the generated sources are generated before doing
# things that might need them.

generate::      $(AstImplSrcs)

# make clean: clear out the generated files so they are
# re-generated at the next build.

clean::
	$X $(AstImplSrcs)

# The "&:" construct is a GNU Make 4.3 (Jan 2020) feature that tells
# Make that running these commands will build all of their outputs. If
# your "make" dislikes this, update to 4.3, or hack up this makefile
# to assure that the $(MAKE) line runs once after the AST generation
# tool has been run, before the AST implementation is compiled.

$(AstImplSrcs) &: $(AstGenSrc)
	$M PACKAGE=$(AstGenPkg) MAIN=$(AstGenMain) -f ../java.mak run ARGS='$(AstImplDir)'

# Verify AstGenerate parameter parsing is working.

ASTTESTS        := $(TOD)args-3.out $(TOD)args-4.out
eachtest::      $(ASTTESTS)

$(TOD)args-3.out:
	$M PACKAGE=$(AstGenPkg) MAIN=$(AstGenMain) INTO=$(TOD)args-3.out -f ../java.mak irun ARGS=''

$(TOD)args-4.out:
	$M PACKAGE=$(AstGenPkg) MAIN=$(AstGenMain) INTO=$(TOD)args-4.out -f ../java.mak irun ARGS='-v $(AstImplDir)'

clean::
	$X $(ARGSTESTS)


endif
endif
endif
endif

# Provide an empty "generate" target so the build keeps
# working even if the auto-generation logic above is
# removed.

generate::                                                      ; @true
.PHONY:         generate
