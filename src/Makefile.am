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

lib_LTLIBRARIES = libconfini.la

libconfini_la_SOURCES = \
	confini.c

libconfini_la_LDFLAGS = \
	-version-info "$(INTERFACE_NUMBER):$(IMPLEMENTATION_NUMBER):$(INTERFACE_COMPATIBILITY)"

libconfini_la_LIBADD = 


if PLATFORM_MSWIN

libconfini_la_LDFLAGS += \
	-no-undefined


if NATIVE_MSWIN

libconfini_la_LDFLAGS += \
	-export-dynamic \
	-avoid-version

endif


if HAVE_RC
	
## Use Microsoft UTF-8 code page (`65001`) when compiling Windows resource files
AM_RCFLAGS = \
	--codepage=65001

libconfini_la_SOURCES += \
	winres.rc

.rc.lo:
	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --tag=RC --mode=compile $(RC) $(AM_RCFLAGS) $(RCFLAGS) $< -o $@

endif


endif


include_HEADERS = \
	confini.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libconfini-$(PACKAGE_VERSION).pc

EXTRA_DIST = libconfini-$(PACKAGE_VERSION).pc.in

## EOF

