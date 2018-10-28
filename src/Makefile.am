## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"


AM_CFLAGS = -Wall -std=c99 -g -O3
lib_LTLIBRARIES = libconfini.la
libconfini_la_SOURCES = confini.c
libconfini_la_LDFLAGS = -version-info 0:0:0
libconfini_la_LIBADD = 


if PLATFORM_MSWIN

libconfini_la_LDFLAGS += \
	-no-undefined \
	-avoid-version


if NATIVE_MSWIN

libconfini_la_LDFLAGS += -export-dynamic

endif


if HAVE_WINDRES

libconfini_la_SOURCES += winres.rc

.rc.lo:
	$(LIBTOOL) --mode=compile windres --codepage=65001 $< -o $@

endif


endif


include_HEADERS = \
	confini.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libconfini-$(PACKAGE_VERSION).pc

EXTRA_DIST = \
	libconfini-$(PACKAGE_VERSION).pc.in

