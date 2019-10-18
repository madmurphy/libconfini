## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	"-DPACKAGE_LOCALE_DIR='$(localedir)'" \
	"-DPACKAGE_SRC_DIR='$(srcdir)'" \
	"-DPACKAGE_DATA_DIR='$(pkgdatadir)'"

AM_CFLAGS = \
	@guessed_am_cflags@ \
	-Wall \
	-pedantic \
	-std=c99 \
	-O@default_optimization@

LTVERSIONINFO = @INTERFACE_NUMBER@:@IMPLEMENTATION_NUMBER@:@INTERFACE_COMPATIBILITY@

lib_LTLIBRARIES = lib@PROJECT_LOCALNAME@.la

lib@PROJECT_LOCALNAME@_la_SOURCES = \
	confini.c

lib@PROJECT_LOCALNAME@_la_LDFLAGS = \
	-version-info '$(LTVERSIONINFO)'

lib@PROJECT_LOCALNAME@_la_LIBADD =


if PLATFORM_MSWIN

lib@PROJECT_LOCALNAME@_la_LDFLAGS += \
	-no-undefined


if NATIVE_MSWIN

lib@PROJECT_LOCALNAME@_la_LDFLAGS += \
	-export-dynamic

endif NATIVE_MSWIN


if HAVE_RC

## Use Microsoft UTF-8 code page (`65001`) when compiling Windows resource files
AM_RCFLAGS = \
	--codepage=65001

lib@PROJECT_LOCALNAME@_la_SOURCES += \
	winres.rc

.rc.lo:
	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --tag=RC --mode=compile $(RC) $(AM_RCFLAGS) $(RCFLAGS) $< -o $@

endif HAVE_RC


endif PLATFORM_MSWIN


if SKIP_VERSIONING

lib@PROJECT_LOCALNAME@_la_LDFLAGS += \
	-avoid-version

endif SKIP_VERSIONING


if WANT_DEVEL

include_HEADERS = \
	@PROJECT_NAME@.h

pkgconfig_DATA = \
	@PACKAGE_LOCALNAME@.pc

endif WANT_DEVEL


EXTRA_DIST = \
	@PACKAGE_TARNAME@.pc.in


## EOF

