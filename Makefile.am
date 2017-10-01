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
	docs/manual.html

man_MANS = docs/man/man3/confini.h.3 \
	docs/man/man3/IniDispatch.3 \
	docs/man/man3/IniFormat.3 \
	docs/man/man3/IniStatistics.3 \
	docs/man/man3/libconfini.3

examplesdir = $(docdir)/examples
examples_DATA = \
	examples/example.conf \
	examples/run_example.sh \
	examples/typed_ini.conf

examplesotherdir = $(examplesdir)/other
examplesother_DATA = \
        examples/other/basic_example.c \
	examples/other/typed_ini.c

examplestopicsdir = $(examplesdir)/topics
examplestopics_DATA = \
	examples/topics/ini_collapse_array.c \
	examples/topics/ini_set_implicit_value.c \
	examples/topics/ini_string_match_si.c \
	examples/topics/ini_unquote.c \
	examples/topics/load_ini_file.c \
	examples/topics/load_ini_path.c

INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)
