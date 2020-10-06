## Process this file with automake to produce Makefile.in


###############################################################################
##    V A R I A B L E S   A N D   G E N E R A L   S E T T I N G S
###############################################################################

AM_CFLAGS = \
	@guessed_am_cflags@ \
	-Wall \
	-pedantic \
	-std=c99

AM_CPPFLAGS = \
	$(TESTSONLY_CPPFLAGS) \
	'-I$(top_srcdir)/src'

check_PROGRAMS = \
	autotest

autotest_SOURCES = \
	autotest.c

autotest_LDADD = \
	$(top_builddir)/src/@PACKAGE_LOCALNAME@.la

TESTS = \
	$(check_PROGRAMS)


###############################################################################
##    R E C I P E S
###############################################################################

# User's rule for forcing non-phony targets to be built, as in `make
# EXPIRE='autotest' autotest` or (equivalently) `make EXPIRE='autotest'
# refresh`
.PHONY: $(EXPIRE) refresh
refresh: $(EXPIRE)


## EOF

