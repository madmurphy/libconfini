## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

EXTRA_DIST = \
	ChangeLog.md \
	MANUAL.md \
	RATIONALE.md \
	README.md \
	autogen.sh \
	autostuff \
	docs \
	mgwmake.bat \
	package.json \
	tests

dist_doc_DATA = \
	AUTHORS \
	COPYING

# Create `$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h` and
# `$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h`
include_insthook_1 = \
	mv "$(DESTDIR)$(includedir)/$(PROJECT_NAME).h" "$(DESTDIR)$(includedir)/$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h" && \
	(cd "$(DESTDIR)$(includedir)" && $(LN_S) "$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h" "$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h")


if THIS_IS_THE_ONLY_VERSION

# Create `$(PROJECT_CONFNAME).h` only when the package must *not* coexist with
# other major versions of itself
include_insthook_2 = \
	(cd "$(DESTDIR)$(includedir)" && $(LN_S) "$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h" "$(PROJECT_CONFNAME).h")

endif


if INCLUDE_DOC

dist_doc_DATA += docs/manual.html \
	ChangeLog \
	NEWS \
	README

# Currently it's not possible to do `notrans_man_MANS = $(MANPAGE_LIST)`,
# because although the variable gets correctly assigned, it results still empty
# when Automake creates the rules, so no rules for man pages will be created. A
# workaround is to make Automake see at least one random file, and then re-set
# the variable properly immediately afterwards, in a way that Automake cannot
# see.
notrans_man_MANS = docs/man/man3/libconfini.3
@POPULATE_MANPAGE_LIST@

endif


if INCLUDE_EXAMPLES

examplesdir = $(docdir)/examples
dist_examples_DATA = \
	examples/run_example.sh

examplesinifilesdir = $(examplesdir)/ini_files
dist_examplesinifiles_DATA = \
	examples/ini_files/ambiguous.conf \
	examples/ini_files/colon_as_delimiter.conf \
	examples/ini_files/bad_ini.conf \
	examples/ini_files/delivery.conf \
	examples/ini_files/extreme_ini.conf \
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
	examples/topics/load_ini_path.c \
	examples/topics/strip_ini_cache.c

exampleutilsdir = $(examplesdir)/utilities
dist_exampleutils_DATA = \
	examples/utilities/load_ini_buffer.h \
	examples/utilities/make_strarray.h


if RENAME_PACKAGE

# Edit example files if the package has been renamed
examples_insthook_1 = \
	$(SED) -i "s/-l$(PROJECT_NAME)/-l$(PROJECT_LOCALNAME)/g" "$(DESTDIR)$(examplesdir)/run_example.sh" && \
	$(FIND) "$(DESTDIR)$(examplesdir)" -type f -name '*.c' | $(XARGS) $(SED) -i 's/<$(PROJECT_NAME).h>/<$(PROJECT_LOCALNAME).h>/g'

else


if !THIS_IS_THE_ONLY_VERSION

# Edit example files if the package must coexist with other major versions of
# itself
examples_insthook_1 = \
	$(SED) -i "s/-l$(PROJECT_NAME)/-l$(PROJECT_DISTNAME)/g" "$(DESTDIR)$(examplesdir)/run_example.sh" && \
	$(FIND) "$(DESTDIR)$(examplesdir)" -type f -name '*.c' | $(XARGS) $(SED) -i 's/<$(PROJECT_NAME).h>/<$(PROJECT_NAME)-$(PROJECT_MAJVER).h>/g'

endif


endif


# Make examples script executable
examples_insthook_2 = \
	chmod +x "$(DESTDIR)$(examplesdir)/run_example.sh"

endif


install-exec-hook:
	$(include_insthook_1)
	$(include_insthook_2)

install-data-hook:
	$(man_insthook)
	$(examples_insthook_1)
	$(examples_insthook_2)

uninstall-local:
	-rm -rf "$(docdir)"
	-rm -f "$(includedir)/$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h" \
		"$(includedir)/$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h"

clean-local:
	$(renaming_cleanhook)
	-rm -rf 'no-dist' "$(CONFNEW_DIR)"

synch:
	$(synch_actions)

dist-hook:
	$(renaming_disthook)


## EOF

