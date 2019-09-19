## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	"-DPACKAGE_LOCALE_DIR=\"$(localedir)\"" \
	"-DPACKAGE_SRC_DIR=\"$(srcdir)\"" \
	"-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\""

AM_CFLAGS = \
	-Wall \
	-pedantic \
	-std=c99 \
	-g \
	"-O$(CODE_OPTIMIZATION)"

lib_LTLIBRARIES = lib@LIBRARY_DISTNAME@.la

lib@LIBRARY_DISTNAME@_la_SOURCES = \
	confini.c

lib@LIBRARY_DISTNAME@_la_LDFLAGS = \
	-version-info "$(INTERFACE_NUMBER):$(IMPLEMENTATION_NUMBER):$(INTERFACE_COMPATIBILITY)"

lib@LIBRARY_DISTNAME@_la_LIBADD = 


if PLATFORM_MSWIN

lib@LIBRARY_DISTNAME@_la_LDFLAGS += \
	-no-undefined


if NATIVE_MSWIN

lib@LIBRARY_DISTNAME@_la_LDFLAGS += \
	-export-dynamic \
	-avoid-version

endif


if HAVE_RC
	
## Use Microsoft UTF-8 code page (`65001`) when compiling Windows resource files
AM_RCFLAGS = \
	--codepage=65001

lib@LIBRARY_DISTNAME@_la_SOURCES += \
	winres.rc

.rc.lo:
	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --tag=RC --mode=compile $(RC) $(AM_RCFLAGS) $(RCFLAGS) $< -o $@

endif


endif


include_HEADERS = \
	confini.h

pkgconfig_DATA = \
	$(PACKAGE_NAME).pc

EXTRA_DIST = \
	$(PACKAGE_REALNAME).pc.in

## EOF

