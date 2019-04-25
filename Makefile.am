## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

EXTRA_DIST = \
	docs \
	autogen.sh \
	README.md \
	ChangeLog.md \
	doxygen.conf \
	dpi.dox \
	MANUAL.md \
	mgwmake.bat \
	package.json \
	RATIONALE.md \
	README.md

dist_doc_DATA = \
	AUTHORS \
	COPYING

# Create `$(LIBRARY_NAME)-$(EFFECTIVE_VERSION).h` and `$(LIBRARY_NAME)-$(LIBRARY_MAJVER).h`
include_hook_1 = \
	mv "$(DESTDIR)$(includedir)/$(LIBRARY_NAME).h" "$(DESTDIR)$(includedir)/$(LIBRARY_NAME)-$(EFFECTIVE_VERSION).h" && \
	(cd "$(DESTDIR)$(includedir)" && $(LN_S) "$(LIBRARY_NAME)-$(EFFECTIVE_VERSION).h" "$(LIBRARY_NAME)-$(LIBRARY_MAJVER).h")


if !MAJOR_VERSIONS_COEXIST

# Create `$(LIBRARY_NAME).h` only when the package must *not* coexist with other major versions of itself
include_hook_2 = \
	(cd "$(DESTDIR)$(includedir)" && $(LN_S) "$(LIBRARY_NAME)-$(EFFECTIVE_VERSION).h" "$(LIBRARY_NAME).h")

endif


if ENABLE_DOC

dist_doc_DATA += docs/manual.html \
	ChangeLog \
	NEWS \
	README

dist_man_MANS = docs/man/man3/confini.h.3 \
	docs/man/man3/IniDispatch.3 \
	docs/man/man3/IniFormat.3 \
	docs/man/man3/IniStatistics.3 \
	docs/man/man3/libconfini.3


if MAJOR_VERSIONS_COEXIST

# Rename man files when the package must coexist with other major versions of itself
man_hook = \
	$(FIND) "$(DESTDIR)$(mandir)" -type f | sed "s|\\([^.]*\\)\\(\\.\\S*\\)|\\0 \\1-$(LIBRARY_MAJVER)\\2|g" | $(XARGS) -n2 mv && \
	$(FIND) "$(DESTDIR)$(mandir)" -type f -name "*$(PACKAGE_REALNAME)-$(LIBRARY_MAJVER)*" | sed "s|\\(\\S*\\)$(PACKAGE_REALNAME)-$(LIBRARY_MAJVER)\\(\\.\\S*\\)|\\0 \\1$(PACKAGE_NAME)\\2|g" | $(XARGS) -n2 mv

endif


endif


if ENABLE_EXAMPLES

examplesdir = $(docdir)/examples
dist_examples_DATA = \
	examples/run_example.sh

examplesinifilesdir = $(examplesdir)/ini_files
dist_examplesinifiles_DATA = \
	examples/ini_files/ambiguous.conf \
	examples/ini_files/colon_as_delimiter.conf \
	examples/ini_files/bad_ini.conf \
	examples/ini_files/delivery.conf \
	examples/ini_files/hash_table.conf \
	examples/ini_files/self_explaining.conf \
	examples/ini_files/typed_ini.conf \
	examples/ini_files/unix-like.conf

examplesotherdir = $(examplesdir)/miscellanea
dist_examplesother_DATA = \
	examples/miscellanea/colon_as_delimiter.c \
	examples/miscellanea/disambiguate.c \
	examples/miscellanea/dispatch_everything.c \
	examples/miscellanea/glib_hash_table.c \
	examples/miscellanea/print_format.c \
	examples/miscellanea/stats_only.c \
	examples/miscellanea/typed_ini.c

examplestopicsdir = $(examplesdir)/topics
dist_examplestopics_DATA = \
	examples/topics/ini_array_break.c \
	examples/topics/ini_array_collapse.c \
	examples/topics/ini_array_foreach.c \
	examples/topics/ini_array_release.c \
	examples/topics/ini_array_shift.c \
	examples/topics/ini_array_split.c \
	examples/topics/ini_global_set_implicit_value.c \
	examples/topics/ini_string_match_si.c \
	examples/topics/ini_string_parse.c \
	examples/topics/load_ini_file.c \
	examples/topics/load_ini_path.c


if MAJOR_VERSIONS_COEXIST

# Edit example files when the package must coexist with other major versions of itself
examples_hook_1 = \
	sed -i "s/-l$(LIBRARY_NAME)/-l$(LIBRARY_LOCALNAME)/g" "$(DESTDIR)$(examplesdir)/run_example.sh" && \
	$(FIND) "$(DESTDIR)$(examplesdir)" -type f -name '*.c' | $(XARGS) sed -i 's/<$(LIBRARY_NAME).h>/<$(LIBRARY_NAME)-$(LIBRARY_MAJVER).h>/g'

endif


# Make example script executable
examples_hook_2 = \
	chmod +x "$(DESTDIR)$(examplesdir)/run_example.sh"

endif


install-exec-hook:
	$(include_hook_1)
	$(include_hook_2)

install-data-hook:
	$(man_hook)
	$(examples_hook_1)
	$(examples_hook_2)

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

## EOF

