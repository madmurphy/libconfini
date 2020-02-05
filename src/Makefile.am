## Process this file with automake to produce Makefile.in


###############################################################################
##    V A R I A B L E S   A N D   G E N E R A L   S E T T I N G S
###############################################################################

# NOTE:  Upper case variables may be freely overridden by the user, while lower
#        case variables, especially those beginning with an underscore, are for
#        internal use only.

LTVERSIONINFO = @INTERFACE_NUMBER@:@IMPLEMENTATION_NUMBER@:@INTERFACE_COMPATIBILITY@

AM_CPPENV = \
	"-DPACKAGE_LOCALE_DIR=\"$(localedir)\"" \
	"-DPACKAGE_SRC_DIR=\"$(srcdir)\"" \
	"-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\""

OFFICIAL_CPPENV = \
	"-DCONFINI_IO_FLAVOR=@parent_cpp_flavor@"

BAREMETAL_CPPENV = \
	"-DCONFINI_IO_FLAVOR=CONFINI_BAREMETAL" \
	"-D_LIBCONFINI_COMPILE_HEADER_=\"confini-baremetal.h\""

AM_CPPFLAGS = \
	$(AM_CPPENV) \
	@AC_CPPENV@

AM_CFLAGS = \
	@guessed_am_cflags@ \
	-Wall \
	-pedantic \
	-std=c99 \
	-O@default_optimization@

MOSTLYCLEANFILES =

CLEANFILES = \
	confini-baremetal.c \
	confini-baremetal.h

lib_LTLIBRARIES = \
	lib@PROJECT_LOCALNAME@.la

lib@PROJECT_LOCALNAME@_la_LDFLAGS = \
	-version-info \
	$(LTVERSIONINFO)

lib@PROJECT_LOCALNAME@_la_LIBADD =

lib@PROJECT_LOCALNAME@_la_SOURCES =

nodist_lib@PROJECT_LOCALNAME@_la_SOURCES =


if WANT_BAREMETAL

BUILT_SOURCES = \
	confini-baremetal.c \
	confini-baremetal.h

nodist_lib@PROJECT_LOCALNAME@_la_SOURCES += \
	confini-baremetal.h \
	confini-baremetal.c

else !WANT_BAREMETAL

lib@PROJECT_LOCALNAME@_la_SOURCES += \
	confini.h \
	confini.c

endif !WANT_BAREMETAL


if PLATFORM_MSWIN

lib@PROJECT_LOCALNAME@_la_LDFLAGS += \
	-no-undefined


if NATIVE_MSWIN

lib@PROJECT_LOCALNAME@_la_LDFLAGS += \
	-export-dynamic

endif NATIVE_MSWIN


if HAVE_DLLTOOL

noinst_SCRIPTS = \
	@lt_cv_objdir@/@PACKAGE_LOCALNAME@.def

endif HAVE_DLLTOOL


if HAVE_RC

AM_RCFLAGS =

nodist_lib@PROJECT_LOCALNAME@_la_SOURCES += \
	winres-ac.rc

endif HAVE_RC


endif PLATFORM_MSWIN


if SKIP_VERSIONING

lib@PROJECT_LOCALNAME@_la_LDFLAGS += \
	-avoid-version

endif SKIP_VERSIONING


if WANT_DEVEL


if WANT_BAREMETAL

nodist_include_HEADERS = \
	built-sources/baremetal/@PROJECT_NAME@.h

else !WANT_BAREMETAL

include_HEADERS = \
	@PROJECT_NAME@.h

endif !WANT_BAREMETAL


pkgconfig_DATA = \
	@PACKAGE_LOCALNAME@.pc

endif WANT_DEVEL


EXTRA_DIST = \
	@PACKAGE_TARNAME@.def \
	@PACKAGE_TARNAME@.pc.in

UNINSTALLFILES =

UNINSTALLHEADERSFILES = \
	'$(DESTDIR)$(includedir)/@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' \
	'$(DESTDIR)$(includedir)/@PROJECT_CONFNAME@-@PROJECT_MAJVER@.h'

_data_insthook_ =

# Create `$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h` and
# `$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h`
_headers_insthook_ = \
	mv '$(DESTDIR)$(includedir)/@PROJECT_NAME@.h' \
	'$(DESTDIR)$(includedir)/@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' && \
	(cd '$(DESTDIR)$(includedir)' && \
	$(LN_S) '@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' \
	'@PROJECT_CONFNAME@-@PROJECT_MAJVER@.h');


if !MULTIVERSION

# Create `$(PROJECT_CONFNAME).h` only when the package must *not* coexist with
# other major versions of itself
_headers_insthook_ += \
	(cd '$(DESTDIR)$(includedir)' && \
	$(LN_S) '@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' \
	'@PROJECT_CONFNAME@.h');

endif !MULTIVERSION


if WANT_DEVEL

# Create `$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h` and
# `$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h`
_data_insthook_ += \
	$(_headers_insthook_)

UNINSTALLFILES += \
	$(UNINSTALLHEADERSFILES)

endif WANT_DEVEL


###############################################################################
##    R E C I P E S
###############################################################################

clean-local:
	-rm -rf 'built-sources';

install-data-hook:
	$(_data_insthook_)

uninstall-local:
	-rm -f $(UNINSTALLFILES);

# Install only the headers
.PHONY: install-headers
install-headers: install-includeHEADERS install-nodist_includeHEADERS
	$(_headers_insthook_)

# Uninstall only the headers
.PHONY: uninstall-headers
uninstall-headers: uninstall-includeHEADERS uninstall-nodist_includeHEADERS
	-rm -f $(UNINSTALLHEADERSFILES);


if HAVE_RC

# Inference rule

.rc.lo:
	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --tag=RC --mode=compile \
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) $< -o $@;

endif HAVE_RC


if HAVE_DLLTOOL

MOSTLYCLEANFILES += \
	*.def_ \
	*.exp_ \
	confini.def \
	confini.exp \
	confini-baremetal.def \
	confini-baremetal.exp

# Inference rules

.@OBJEXT@.def_:
	$(DLLTOOL) -z '$@' --export-all-symbols '$<' && sed -i '/^\s*\;/{d};' \
	'$@';

.@OBJEXT@.exp_:
	$(DLLTOOL) -e '$@' --export-all-symbols '$<';

@lt_cv_objdir@/@PACKAGE_LOCALNAME@.def: lib@PROJECT_LOCALNAME@.la
	$(DLLTOOL) -z '$@' --export-all-symbols \
	'@lt_cv_objdir@/lib@PROJECT_LOCALNAME@.a';

@lt_cv_objdir@/@PACKAGE_LOCALNAME@.exp: lib@PROJECT_LOCALNAME@.la
	$(DLLTOOL) -e '$@' --export-all-symbols \
	'@lt_cv_objdir@/lib@PROJECT_LOCALNAME@.a';

# Ordinary targets

confini.def confini.exp: \
 confini.c \
 confini.h
	$(MAKE) AM_CPPFLAGS='$(AM_CPPENV) $(OFFICIAL_CPPENV)' '$@_' && mv \
	'$@_' '$@';

confini-baremetal.def confini-baremetal.exp: \
 confini-baremetal.c \
 confini-baremetal.h
	$(MAKE) AM_CPPFLAGS='$(AM_CPPENV) $(BAREMETAL_CPPENV)' '$@_' && mv \
	'$@_' '$@';

endif HAVE_DLLTOOL


if AUTHOR_MODE

# Accept `dev/marked-sources/confini-marked.c` without amendments
confini.c: \
 $(top_srcdir)/dev/marked-sources/confini-marked.c \
 $(top_srcdir)/dev/hackings/all/confini-metadata.c
	@@confini_c_recipe@

# Accept `dev/marked-sources/confini-marked.h` without amendments
confini.h: \
 $(top_srcdir)/dev/marked-sources/confini-marked.h \
 $(top_srcdir)/dev/hackings/all/confini-metadata.h
	@@confini_h_recipe@


if HAVE_DLLTOOL

@PACKAGE_TARNAME@.def: \
 confini.c \
 confini.h
	$(MAKE) AM_CPPFLAGS='$(AM_CPPENV) $(OFFICIAL_CPPENV)' 'confini.def_' \
	&& mv 'confini.def_' '$@';

endif HAVE_DLLTOOL


endif AUTHOR_MODE


if REVISE_BAREMETAL

$(top_builddir)/no-dist/.archived/$(nonportfile):
	@$(MAKE) -C '$(top_builddir)' 'no-dist/.archived/$(nonportfile)';

$(top_builddir)/no-dist/hackings/baremetal/c-standard-library.h: \
 $(top_builddir)/no-dist/.archived/$(nonportfile)
	@$(MAKE) -C '$(top_builddir)' \
	'no-dist/hackings/baremetal/c-standard-library.h';

endif REVISE_BAREMETAL


# Amend `dev/marked-sources/confini-marked.c`
built-sources/baremetal/confini.c: \
 $(top_srcdir)/dev/marked-sources/confini-marked.c \
 $(top_srcdir)/dev/hackings/all/confini-metadata.c \
 $(top_srcdir)/dev/hackings/baremetal/confini-header.c \
 $(top_srcdir)/dev/hackings/baremetal/number-parsers.c \
 $(top_srcdir)/dev/hackings/baremetal/pp-utils.c
	@@confini_baremetal_c_recipe@

# Amend `dev/marked-sources/confini-marked.h`
built-sources/baremetal/confini.h: \
 $(top_srcdir)/dev/marked-sources/confini-marked.h \
 $(top_srcdir)/dev/hackings/all/confini-metadata.h \
 $(top_srcdir)/dev/hackings/baremetal/number-parsers.h \
 $(top_builddir)/no-dist/hackings/baremetal/c-standard-library.h
	@@confini_baremetal_h_recipe@

# Symbolic link
confini-baremetal.c: \
 built-sources/baremetal/confini.c
	@rm -f '$@' && $(LN_S) '$<' '$@'

# Symbolic link
confini-baremetal.h: \
 built-sources/baremetal/confini.h
	@rm -f '$@' && $(LN_S) '$<' '$@'

# User's rule for forcing non-phony targets to be built, as in `make
# EXPIRE='confini.c confini.h' confini.c confini.h` or (equivalently) `make
# EXPIRE='confini.c confini.h' expired`
.PHONY: $(EXPIRE) expired
expired: $(EXPIRE)


## EOF

