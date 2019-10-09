## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

EXTRA_DIST = \
	ChangeLog.md \
	MANUAL.md \
	README.md \
	autostuff \
	bootstrap \
	docs \
	mgwmake.bat \
	package.json \
	tests

dist_doc_DATA = \
	AUTHORS \
	COPYING

# Create `$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h` and
# `$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h`
header_insthook_1 = \
	mv '$(DESTDIR)$(includedir)/$(PROJECT_NAME).h' \
	'$(DESTDIR)$(includedir)/$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h' && \
	(cd '$(DESTDIR)$(includedir)' && \
	$(LN_S) '$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h' \
	'$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h')


if !MULTIVERSION

# Create `$(PROJECT_CONFNAME).h` only when the package must *not* coexist with
# other major versions of itself
header_insthook_2 = \
	(cd '$(DESTDIR)$(includedir)' && \
	$(LN_S) '$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h' \
	'$(PROJECT_CONFNAME).h')

endif


if INCLUDE_DOC

dist_doc_DATA += docs/manual.html \
	ChangeLog \
	NEWS \
	README

# Currently it is not possible to do `notrans_man_MANS = $(MANPAGE_LIST)`,
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


if HAVE_TRANSFORMATIONS

# Update example files if the package must coexist with other major versions of
# itself
examples_insthook_1 = \
	$(SED) -i 's/-l$(PROJECT_NAME)/-l$(PROJECT_LOCALNAME)/g' \
	'$(DESTDIR)$(examplesdir)/run_example.sh' && \
	$(FIND) '$(DESTDIR)$(examplesdir)' -type f -name '*.c' | \
	$(XARGS) $(SED) -i 's/<$(PROJECT_NAME).h>/<$(LIBRARY_HEADERNAME).h>/g'

endif


# Make examples script executable
examples_insthook_2 = \
	chmod +x '$(DESTDIR)$(examplesdir)/run_example.sh'

endif


# Primary targets

install-exec-hook:
	$(header_insthook_1)
	$(header_insthook_2)

install-data-hook:
	$(man_insthook)
	$(examples_insthook_1)
	$(examples_insthook_2)

uninstall-local:
	-rm -f '$(DESTDIR)$(includedir)/$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h' \
	'$(DESTDIR)$(includedir)/$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h'
	-rm -rf '$(DESTDIR)$(docdir)'

clean-local:
	$(renaming_cleanhook)
	-rm -rf 'no-dist' '$(CONFNEW_DIR)'

distclean-local:
	-rm -f 'make-targets.txt'

.PHONY: synch
synch:
	$(AM_V_GEN)$(synch_actions)

dist-hook:
	$(renaming_disthook)

.PHONY: author-clean
author-clean: maintainer-clean
	rm -rf '$(srcdir)/autom4te.cache' '$(srcdir)/m4' \
	'$(srcdir)/build-aux' `find '$(srcdir)' -type d -name .deps` `find . -type d -name .deps`
	rm -f '$(srcdir)/aclocal.m4' '$(srcdir)'/config.* config.* '$(srcdir)/configure' \
	'$(srcdir)/libtool' 'libtool' '$(srcdir)/ltmain.sh' 'ltmain.sh' `find '$(srcdir)' -name 'Makefile.in'`


# Secondary targets

.PHONY: binary-image
binary-image:
	_DSTDIR_="`pwd`/$(PACKAGE_LOCALNAME)-$(PACKAGE_VERSION)-$(build)-pkg/" && \
	rm -rf "$$_DSTDIR_" && $(MAKE) DESTDIR="$$_DSTDIR_" install

.PHONY: source-image
source-image: distdir
	rm -f '$(distdir)/ChangeLog.md' '$(distdir)/README.md' && \
	(cd '$(distdir)' && $(LN_S) 'ChangeLog' 'ChangeLog.md' && \
	$(LN_S) 'README' 'README.md')

.PHONY: authors-suitcase
authors-suitcase: source-image
	(cd '$(distdir)' && rm -rf build-aux m4 && rm -f aclocal.m4 \
	'configure' `find . -name Makefile.in`) && \
	mv '$(distdir)' '$(distdir)-dev'

.PHONY: binary-release
binary-release: binary-image
	(cd '$(PACKAGE_LOCALNAME)-$(PACKAGE_VERSION)-$(build)-pkg' && \
	$(AMTAR) -cJf '../$(PACKAGE_LOCALNAME)-$(PACKAGE_VERSION)-$(build).pkg.tar.xz' *) && \
	rm -rf '$(PACKAGE_LOCALNAME)-$(PACKAGE_VERSION)-$(build)-pkg'

.PHONY: source-release
source-release: source-image
	$(AMTAR) -czf '$(distdir).tar.gz' '$(distdir)' && rm -rf '$(distdir)'

.PHONY: authors-copy
authors-copy: authors-suitcase
	$(AMTAR) -czf '$(distdir)-dev.tar.gz' '$(distdir)-dev' && \
	rm -rf '$(distdir)-dev'

.PHONY: install-manifest
install-manifest: binary-image
	(cd '$(PACKAGE_LOCALNAME)-$(PACKAGE_VERSION)-$(build)-pkg' && \
	$(FIND) * -type f,l -printf '/%p\n' > \
	'../$(PACKAGE_LOCALNAME)-$(PACKAGE_VERSION)-$(build).manifest') && \
	rm -rf '$(PACKAGE_LOCALNAME)-$(PACKAGE_VERSION)-$(build)-pkg'

.PHONY: clobber
clobber: uninstall clean

.PHONY: help
help:
	$(AM_V_GEN)cat 'make-targets.txt' && \
	echo 'This package is currently configured for a $(host_cpu) $(host_vendor) running $(host_os).' || \
	echo 'Please launch `$(srcdir)/configure` again.'


## EOF

