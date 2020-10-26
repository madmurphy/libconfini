## Process this file with automake to produce Makefile.in


###############################################################################
##    V A R I A B L E S   A N D   G E N E R A L   S E T T I N G S
###############################################################################

# NOTE:  Upper case variables may be freely overridden by the user, while lower
#        case variables, especially those beginning with an underscore, are for
#        internal use only.

LTVERSIONINFO = $(INTERFACE_NUM):$(IMPLEMENTATION_NUM):$(INTERFACES_SUPPORTED)

_common_cppenv_ = \
	"-DPACKAGE_LOCALE_DIR=\"$(localedir)\"" \
	"-DPACKAGE_SRC_DIR=\"$(srcdir)\"" \
	"-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\""

_with_io_cppenv_ = \
	"-DCONFINI_IO_FLAVOR=@na_io_api@"

_baremetal_cppenv_ = \
	"-DCONFINI_IO_FLAVOR=CONFINI_BAREMETAL"

AM_CPPFLAGS = \
	$(_common_cppenv_) \
	@na_cppenv@

AM_CFLAGS = \
	@guessed_am_cflags@ \
	-Wall \
	-pedantic \
	-std=c99 \
	-O@default_optimization@

MOSTLYCLEANFILES =

CLEANFILES = \
	confini-baremetal.c \
	confini-baremetal.h \
	libconfini-baremetal.def

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


if WANT_DLLEXP

nodist_noinst_DATA = \
	@lt_cv_objdir@/@PACKAGE_LOCALNAME@.def \
	@lt_cv_objdir@/@PACKAGE_LOCALNAME@.exp

endif WANT_DLLEXP


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
	@PACKAGE_TARNAME@.pc.in \
	winres.rc

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


LOCALSYMBOLS = @PACKAGE_LOCALNAME@-current.def


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
	confini.def \
	confini.exp \
	confini-baremetal.def \
	confini-baremetal.exp

# Inference rules

.a.def:
	$(DLLTOOL) -z '$@' --export-all-symbols '$<' && $(SED) -i '/^\s*\;/{d};' \
	'$@';

.@OBJEXT@.def:
	$(DLLTOOL) -z '$@' --export-all-symbols '$<' && $(SED) -i '/^\s*\;/{d};' \
	'$@';

.a.exp:
	$(DLLTOOL) -e '$@' --export-all-symbols '$<';

.@OBJEXT@.exp:
	$(DLLTOOL) -e '$@' --export-all-symbols '$<';

 .PHONY: current-symbols
 current-symbols: lib@PROJECT_LOCALNAME@.la
	@$(MAKE) '@lt_cv_objdir@/lib@PROJECT_LOCALNAME@.def' && \
	cp '@lt_cv_objdir@/lib@PROJECT_LOCALNAME@.def' '$(LOCALSYMBOLS)';


endif HAVE_DLLTOOL


if AUTHOR_MODE

# Accept `dev/marked-sources/confini-marked.c` without amendments
$(srcdir)/confini.c: \
 $(top_srcdir)/dev/marked-sources/confini-marked.c \
 $(top_srcdir)/dev/hackings/all/confini-metadata.c \
 $(top_srcdir)/dev/hackings/baremetal/confini-header.c
	@@confini_c_recipe@

# Accept `dev/marked-sources/confini-marked.h` without amendments
$(srcdir)/confini.h: \
 $(top_srcdir)/dev/marked-sources/confini-marked.h \
 $(top_srcdir)/dev/hackings/all/confini-metadata.h
	@@confini_h_recipe@


if !WANT_BAREMETAL


if HAVE_DLLTOOL

$(srcdir)/@PACKAGE_TARNAME@.def: $(srcdir)/confini.c $(srcdir)/confini.h
	@$(MAKE) LOCALSYMBOLS='$@' current-symbols;

endif HAVE_DLLTOOL


endif !WANT_BAREMETAL


endif AUTHOR_MODE


if REVISE_LIBC

$(top_builddir)/no-dist/.archived/$(nonportfile):
	@$(MAKE) -C '$(top_builddir)' 'no-dist/.archived/$(nonportfile)';

$(top_builddir)/no-dist/hackings/baremetal/c-standard-library.h: \
 $(top_builddir)/no-dist/.archived/$(nonportfile)
	@$(MAKE) -C '$(top_builddir)' \
	'no-dist/hackings/baremetal/c-standard-library.h';

endif REVISE_LIBC


if WANT_BAREMETAL


if HAVE_DLLTOOL

@PACKAGE_TARNAME@-baremetal.def: confini-baremetal.c confini-baremetal.h
	@$(MAKE) LOCALSYMBOLS='$@' current-symbols;

endif HAVE_DLLTOOL


endif WANT_BAREMETAL


# Amend `dev/marked-sources/confini-marked.c`
built-sources/baremetal/confini.c: \
 $(top_srcdir)/dev/marked-sources/confini-marked.c \
 $(top_srcdir)/dev/hackings/all/confini-metadata.c \
 $(top_srcdir)/dev/hackings/baremetal/str2num.c \
 $(top_srcdir)/dev/hackings/baremetal/confini-header.c
	@@confini_baremetal_c_recipe@

# Amend `dev/marked-sources/confini-marked.h`
built-sources/baremetal/confini.h: \
 $(top_srcdir)/dev/marked-sources/confini-marked.h \
 $(top_srcdir)/dev/hackings/all/confini-metadata.h \
 $(top_srcdir)/dev/hackings/baremetal/str2num.h \
 $(top_builddir)/no-dist/hackings/baremetal/c-standard-library.h
	@@confini_baremetal_h_recipe@

# Symbolic link
confini-baremetal.c: \
 built-sources/baremetal/confini.c
	@rm -f '$@' && $(LN_S) '$<' '$@';

# Symbolic link
confini-baremetal.h: \
 built-sources/baremetal/confini.h
	@rm -f '$@' && $(LN_S) '$<' '$@';

# User's rule for forcing non-phony targets to be built, as in `make
# EXPIRE='confini.c confini.h' confini.c confini.h` or (equivalently) `make
# EXPIRE='confini.c confini.h' refresh`
.PHONY: $(EXPIRE) refresh
refresh: $(EXPIRE)


## EOF

