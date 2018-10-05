## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	-Wall\
	-std=c99\
	-g\
	-O3

lib_LTLIBRARIES = libconfini.la


libconfini_la_SOURCES = \
	confini.c

libconfini_la_LDFLAGS = 

libconfini_la_LIBADD = 


if PLATFORM_WIN32
libconfini_la_LDFLAGS += -no-undefined
endif

if NATIVE_WIN32
libconfini_la_LDFLAGS += -export-dynamic
endif

include_HEADERS = \
	confini.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = confini-1.7.pc

EXTRA_DIST = \
	confini-1.7.pc.in

