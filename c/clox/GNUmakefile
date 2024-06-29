# -*- Makefile -*-

# This C project builds a program called "clox"
MAIN            := clox

# I will be telling GCC to constrain us to C99
# (formal title, INCITS ISO/IEC 9899:1999)
# until original author deviates significantly.

CC              := c99

# NOTE: this is GCC-specific.
# COPTS           := -fshort-enums

# This subproject traffics in log files.
LOGD            := log/

# Inherit recurive build behaviors from parent
include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

# tests: run power-on and built-in self tests.
#
# NOTE: a nonzero exit status from BIST is now treated
# as a build failure.

tests::         $(BIND)$(MAIN)
	$P '\n'
	$P '  %-14s\n' "POST and BIST"
	$Q $(BIND)$(MAIN) bist
	$P '\n'

# format: reformat all source files.
#
# depends on being able to build the program.
#
# This should ALWAYS be done in a Git branch as part of the last
# commit before merging the branch back into the trunk.

format::        $(BIND)$(MAIN)
	$C ${BIND}indent-c.sh $(SRCD)*.c
	$C ${BIND}indent-h.sh $(INCD)*.h

# docs: update automatically generated documentation.
# This subproject uses DOXYGEN to extract API documents
# from the source code, and is configured to create
# a tree of HTML files and a PDF.
#
# To generate the PDF, you will need to install
# - texlive, to get pdflatex and friends
# - texlive-latex-extra, for the "varwidth" style file
# installation package names may vary, the above are the
# package names for "Pop! OS" (an Ubuntu variant).

docs::          doc/html/index.html doc/latex/refman.pdf

clean::
	$X doc/html/search/*
	@ [ ! -d doc/html/search ] || $Y doc/html/search
	$X doc/html/* doc/latex/*

doc/html/index.html:    doc/Doxyfile $(BIND)$(MAIN)
	$C [ -d log ] || mkdir -p log
	$P '  %-14s %s\n' "DOXYGEN" "doc/html"
	doxygen doc/Doxyfile > $(LOGD)doxygen.log 2>$(LOGD)doxygen.err

doc/latex/refman.pdf:   doc/html/index.html
	$P '  %-14s %s\n' "PDFLATEX" "doc/refman.pdf"
	make -C doc/latex pdf > $(LOGD)latex.log 2>$(LOGD)latex.err

# The presence of "STUB ();" calls is not a build error, but expose
# where they all are to point out what needs to be implemented next.

stubs::
	$P '\n'
	$P '  %-14s %s\n' "FIND STUBs" "$(INCD)*.h $(SRCD)*.c"
	$C grep -ni '\<stub\>' $(SRCD)*.c $(INCD)*.h /dev/null || echo '... no STUBs found.'
	$P '\n'

# The presence of "TODO" comments is not a build error, but expose
# where they all are to point out what needs to be implemented soon.

todos::
	$P '\n'
	$P '  %-14s %s\n' "FIND TODOs" "$(INCD)*.h $(SRCD)*.c"
	$C grep -ni '\<todo\>' $(SRCD)*.c $(INCD)*.h /dev/null || echo '... no TODOs found.'
	$P '\n'

# Maintain a "TAGS" file for emacs.

etags::		TAGS			; $T

TAGS::		$(INCS) $(SRCS) GNUmakefile
	$P '  %-14s %s\n' "TAGS" "$(INCD)*.h $(SRCD)*.c"
	$Q etags $(INCS) $(SRCS)

# Maintain a "tags" file for vi.

ctags::		tags			; $T

tags::		$(INCS) $(SRCS) GNUmakefile
	$P '  %-14s %s\n' "tags" "$(INCD)*.h $(SRCD)*.c"
	$Q ctags $(INCS) $(SRCS)

# The sequence that I want to run when I have made a change
# where everything SHOULD be good but something MIGHT not be good.

clean::
	$X $(LOGD)*.out $(LOGD)*.err $(LOGD)*.log

cycle::
	$M -k -j $(BIND)$(MAIN)
	$M format
	$M tests
	$M etags ctags
	$M docs
	$M stubs
	$M todos

world::
	$M cycle
