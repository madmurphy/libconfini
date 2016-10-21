## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

dist_doc_DATA = \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS \
	README \
	MAN.md

man_MANS = docs/man/man3/confini.h.3 \
	docs/man/man3/libconfini.3

examplesdir = $(docdir)/examples
examples_DATA = \
        examples/compile_example.sh \
	examples/example.c \
	examples/example.conf


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$(examples_DATA) \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)