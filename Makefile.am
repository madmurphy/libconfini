## Process this file with automake to produce Makefile.in


###############################################################################
##    V A R I A B L E S   A N D   G E N E R A L   S E T T I N G S
###############################################################################

# NOTE:  Upper case variables may be freely overridden by the user, while lower
#        case variables, especially those beginning with an underscore, are for
#        internal use only.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	tests \
	src

EXTRA_DIST = \
	MANUAL.md \
	README.md \
	autostuff \
	bootstrap \
	dev \
	docs \
	mgwmake.bat \
	package.json \
	.editorconfig

dist_doc_DATA = \
	AUTHORS \
	COPYING

_am_installalso_ =

_examples_insthook_ =


if WANT_DEVEL


if WANT_DOC

dist_doc_DATA += \
	ChangeLog \
	NEWS \
	README \
	docs/manual.html


if WANT_MAN

notrans_man3_MANS = \
	$(MAN3PAGES)

endif WANT_MAN


if WANT_HTML

_am_installalso_ += \
	install-html;

endif WANT_HTML


endif WANT_DOC


if WANT_EXAMPLES

examplesdir = $(docdir)/examples
dist_examples_SCRIPTS = \
	examples/run-example.sh

examplescppdir = $(examplesdir)/cplusplus
dist_examplescpp_DATA = \
	examples/cplusplus/map.cpp \
	examples/cplusplus/map.hpp \
	examples/cplusplus/generic.cpp

examplesinifilesdir = $(examplesdir)/ini_files
dist_examplesinifiles_DATA = \
	examples/ini_files/ambiguous.conf \
	examples/ini_files/colon_as_delimiter.conf \
	examples/ini_files/bad_ini.conf \
	examples/ini_files/delivery.conf \
	examples/ini_files/extreme_ini.conf \
	examples/ini_files/log.ini \
	examples/ini_files/hash_table.conf \
	examples/ini_files/pacman.conf \
	examples/ini_files/self_explaining.conf \
	examples/ini_files/toml-like.conf \
	examples/ini_files/typed_ini.conf \
	examples/ini_files/unix-like.conf

examplesmiscdir = $(examplesdir)/miscellanea
dist_examplesmisc_DATA = \
	examples/miscellanea/colon_as_delimiter.c \
	examples/miscellanea/disambiguate.c \
	examples/miscellanea/dispatch_everything.c \
	examples/miscellanea/glib_hash_table.c \
	examples/miscellanea/parse_foreign.c \
	examples/miscellanea/print_format.c \
	examples/miscellanea/stats_only.c \
	examples/miscellanea/toml-like.c \
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
	examples/utilities/clone_ini_dispatch.h \
	examples/utilities/ini_string_preparse.h \
	examples/utilities/load_ini_buffer.h \
	examples/utilities/make_strarray.h


if HAVE_TRANSFORMATIONS

# Update example files if the package must coexist with other major versions of
# itself or if it has been renamed
_examples_insthook_ += \
	$(SED) -i 's/-l@PROJECT_NAME@/-l@PROJECT_LOCALNAME@/g' \
	'$(DESTDIR)$(examplesdir)/run-example.sh' && \
	$(FIND) '$(DESTDIR)$(examplesdir)' -type f -name '*.c' -exec \
	$(SED) -i 's/<@PROJECT_NAME@.h>/<@library_headername@.h>/g' {} \;;

endif HAVE_TRANSFORMATIONS


endif WANT_EXAMPLES


endif WANT_DEVEL


# Files/directories erased by `make oblivion-clean`

BINPATH = @PACKAGE_CONFNAME@-@PACKAGE_VERSION@-@host@-pkg

PTBPATH = @PACKAGE_CONFNAME@-@PACKAGE_VERSION@-@host@-builds

SRCPATH = @PACKAGE_TARNAME@-@PACKAGE_VERSION@-src

DEVPATH = @PACKAGE_TARNAME@-@PACKAGE_VERSION@-dev

GITPATH = @PACKAGE_TARNAME@-git

BAKPATH = @PACKAGE_TARNAME@-snapshot


# Other settings

MOSTLYCLEANFILES = \
	$(SANDBOX).lst $(SANDBOX).tar $(SANDBOX).tar.gz $(SANDBOX).tar.xz \
	$(SANDBOX).tgz $(SANDBOX).txz $(SANDBOX).zip

CLEANFILES = \
	dev/tests/null-byte_injection/null-byte_injected.conf \
	dev/tests/performance/big_file.ini \
	dev/tests/performance/speedtest

# Shell expansion is supported here
_bootstrapclean_dirs_ = \
	'autom4te.cache' \
	'build-aux' \
	'm4' \
	`$(FIND) -L . -type d -name .deps`

# Shell expansion is supported here
_bootstrapclean_files_ = \
	'aclocal.m4' \
	'config'.* \
	'configure' \
	'libtool' \
	'ltmain.sh' \
	`$(FIND) -L . -type f -name 'Makefile.in'`

# Shell expansion is supported here
_oblivionclean_dirs_ = \
	'$(distdir)' '$(BINPATH)' '$(PTBPATH)' '$(SRCPATH)' '$(DEVPATH)' \
	'$(BAKPATH)' '$(GITPATH)'

# Shell expansion is supported here
_oblivionclean_files_ = \
	'$(distdir).shar.gz' '$(distdir).tar.bz2' '$(distdir).tar.gz' \
	'$(distdir).tar.lz' '$(distdir).tar.xz' '$(distdir).tar.Z' \
	'$(distdir).zip' '$(BINPATH).lst' '$(BINPATH).tar' \
	'$(BINPATH).tar.gz' '$(BINPATH).tar.xz' '$(BINPATH).tgz' \
	'$(BINPATH).txz' '$(BINPATH).zip' '$(PTBPATH).tar' \
	'$(PTBPATH).tar.gz' '$(PTBPATH).tar.xz' '$(PTBPATH).tgz' \
	'$(PTBPATH).txz' '$(PTBPATH).zip' '$(SRCPATH).tar' \
	'$(SRCPATH).tar.gz' '$(SRCPATH).tar.xz' '$(SRCPATH).tgz' \
	'$(SRCPATH).txz' '$(SRCPATH).zip' '$(DEVPATH).tar' \
	'$(DEVPATH).tar.gz' '$(DEVPATH).tar.xz' '$(DEVPATH).tgz' \
	'$(DEVPATH).txz' '$(DEVPATH).zip' '$(BAKPATH)'*'.tar' \
	'$(BAKPATH)'*'.tar.gz' '$(BAKPATH)'*'.tgz' '$(BAKPATH)'*'.tar.xz' \
	'$(BAKPATH)'*'.txz' '$(BAKPATH)'*'.zip' '$(GITPATH).tar' \
	'$(GITPATH).tar.gz' '$(GITPATH).tar.xz' '$(GITPATH).tgz' \
	'$(GITPATH).txz' '$(GITPATH).zip'

INSTALLCHECKS_CPPENV = \
	$(TESTSONLY_CPPFLAGS) \
	"-DTESTS_CONFINI_HEADER=<@library_headername@.h>"

# Default compression format (one word only allowed)
ZIPTYPE = tar.gz

# The `install` target to use by default (see, for example `make alldone` and
# `make binary-release` -- one word only allowed)
INSTALLFLAVOR = install-strip

# Default space-separated list of actions for `make beforedone` -- do not
# change this variable here; although the the user can freely change it from
# the command line, in the `Makefile` it must contain targets that support the
# `DESTDIR` variable
FINALJOBS = \
	installcheck

# Default target for `make volatile*` (one word only allowed) -- do not change
# this variable here; although the the user can freely change it from the
# command line, in the `Makefile` it must have this value
RUNJOB = $(INSTALLFLAVOR)

# Sandbox default path (one word only allowed)
SANDBOX = volatile

GITFLAGS = \
	--quiet \
	--progress


###############################################################################
##    R E C I P E S   P A R T   I :   D E V E L O P M E N T   A N D   U S A G E
###############################################################################

# Supplementary targets for GNU Automake

install-html-local: $(srcdir)/docs/html $(srcdir)/docs/index.html
	@(cd '$(srcdir)/docs' && $(FIND) -L 'html' -type d -exec $(MKDIR_P) \
	'$(DESTDIR)$(htmldir)/{}' \; && $(FIND) -L 'html' -type f -exec \
	$(INSTALL_DATA) {} '$(DESTDIR)$(htmldir)/{}' \;) && mv \
	'$(DESTDIR)$(htmldir)/html' '$(DESTDIR)$(htmldir)/refbook';

install-data-local: $(_am_installalso_)

install-data-hook:
	$(_examples_insthook_)

uninstall-local:
	-rm -rf '$(DESTDIR)$(htmldir)' '$(DESTDIR)$(docdir)';

installcheck-local:
	@$(MAKE) -C 'tests' clean && $(MAKE) \
	AM_CPPFLAGS='$(INSTALLCHECKS_CPPENV) -I$(DESTDIR)$(includedir)' \
	autotest_LDADD='-L$(DESTDIR)$(libdir) -l@PROJECT_LOCALNAME@' check;

mostlyclean-local:
	-rm -rf '$(SANDBOX)' '._'*;

clean-local:
	@transf_clean_recipe@
	@revision_clean_recipe@

distclean-local:
	@transf_distclean_recipe@
	-rm -rf 'help' 'no-dist' '@confnewdir@' '@rvdir@';

dist-hook:
	@transf_dist_recipe@


# Novel targets

# Install only the stripped binaries
.PHONY: install-exec-strip
install-exec-strip:
	@if test -z '$(STRIP)'; then $(MAKE) $(AM_MAKEFLAGS) \
	INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	install-exec; else $(MAKE) $(AM_MAKEFLAGS) \
	INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	"INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'" install-exec; fi;

# Install/uninstall only the headers
.PHONY: install-headers uninstall-headers
install-headers uninstall-headers:
	@$(MAKE) -C 'src' '$@';

# Check that `$(libdir)` and `$(includedir)` work correctly
.PHONY: finishcheck
finishcheck:
	@$(MAKE) -C 'tests' clean && $(MAKE) \
	AM_CPPFLAGS='$(INSTALLCHECKS_CPPENV)' \
	autotest_LDADD='-l@PROJECT_LOCALNAME@' check;

# Let us decide whether stripping or not
.PHONY: nearlydone
nearlydone: $(INSTALLFLAVOR)

# Defaults to `make installcheck` (mostly `Makefile` glue)
.PHONY: beforedone
beforedone: $(FINALJOBS)

# Calls `make $(INSTALLFLAVOR)` followed by `make beforedone`
.PHONY: alldone
alldone: $(INSTALLFLAVOR)
	@$(MAKE) beforedone;

# Make the source directory depend on Autotools and a `bootstrap` script
.PHONY: bootstrap-clean
bootstrap-clean: maintainer-clean
	-rm -rf $(_bootstrapclean_dirs_); rm -f $(_bootstrapclean_files_); \
	(cd '$(srcdir)' && rm -rf $(_bootstrapclean_dirs_); \
	rm -f $(_bootstrapclean_files_));

# Call `make bootstrap-clean`, then remove all the archives created
.PHONY: oblivion-clean
oblivion-clean: bootstrap-clean
	rm -rf $(_oblivionclean_dirs_); rm -f $(_oblivionclean_files_);

# Merge the files created by `./configure --enable-extended-config=sandbox` and
# update the source code
.PHONY: mergenew
mergenew:
	@@mergenew_recipe@
	@$(MAKE) all-official-sources;

# Uninstall what `make install` has created, then clean the source directory
.PHONY: clobber
clobber: uninstall clean

# Print the list of common targets
.PHONY: help
help:
	@test -f 'help/make-targets.txt' && cat 'help/make-targets.txt' || \
	(echo 'Not available. Please launch `$(srcdir)/configure` again.' \
	1>&2; exit 2);

# User's rule for forcing non-phony targets to be built, as in  `make
# EXPIRE='libconfini-git.tar.gz libconfini-snapshot.tar.gz'
# libconfini-git.tar.gz libconfini-snapshot.tar.gz` or (equivalently) `make
# EXPIRE='libconfini-git.tar.gz libconfini-snapshot.tar.gz' refresh`
.PHONY: $(EXPIRE) refresh
refresh: $(EXPIRE)


###############################################################################
##    R E C I P E S   P A R T   I I:   H A C K I N G S
###############################################################################

OFFICIALSRC =

BAREMETALSRC = \
	baremetal-source-code


if AUTHOR_MODE

OFFICIALSRC += \
	official-source-code

 # (Re-)create the official source files
 .PHONY: official-source-code
 official-source-code:
	$(MAKE) -C 'src' 'confini.h' 'confini.c';


if !WANT_BAREMETAL


if HAVE_DLLTOOL

OFFICIALSRC += \
	official-symbols

 # (Re-)create the official exported symbols
 .PHONY: official-symbols
 official-symbols:
	$(MAKE) -C 'src' '@PACKAGE_TARNAME@.def';

endif HAVE_DLLTOOL


endif !WANT_BAREMETAL


 # (Re-)create the distributed source files (use `./configure
 # --enable-author-mode` for this)
 .PHONY: all-official-sources
 all-official-sources: $(OFFICIALSRC)


endif AUTHOR_MODE


# Create the "bare metal" version of the source files (launch `./configure
# --wihout-io-api` to use it)
.PHONY: baremetal-source-code
baremetal-source-code:
	$(MAKE) -C 'src' 'confini-baremetal.h' 'confini-baremetal.c';


if WANT_BAREMETAL


if HAVE_DLLTOOL

BAREMETALSRC += \
	baremetal-symbols

 # Create the baremetal exported symbols
 .PHONY: baremetal-symbols
 baremetal-symbols:
	$(MAKE) -C 'src' '@PACKAGE_TARNAME@-baremetal.def';

endif HAVE_DLLTOOL


endif WANT_BAREMETAL


# Create the "bare metal" version of the source files (launch `./configure
# --wihout-io-api` to use it)
.PHONY: all-baremetal-sources
all-baremetal-sources: $(BAREMETALSRC)

# (Re-)create all source files
.PHONY: all-sources
all-sources: $(OFFICIALSRC) $(BAREMETALSRC)


if REVISE_LIBC

CLEANFILES += \
	no-dist/hackings/baremetal/c-standard-library.h

 # Non-phony special prerequisites

 # Defaults to `no-dist/.archived/non-portable.h`
 no-dist/.archived/$(nonportfile): no-dist/conf-results/stdmissing.h
	@if ! test -f '$(rvdir)/$(nonportfile)'; then $(MKDIR_P) '$(rvdir)'; \
	cp 'no-dist/conf-results/stdmissing.h' '$(rvdir)/$(nonportfile)'; { \
	echo; echo \
	'**Error** I could not recovery the current revision. I am going to use the'; \
	echo 'output from the `configure` script to regenerate it...'; echo; \
	echo \
	'Please revise the content of the `$(rvdir)` subdirectory, then try again.'; \
	echo; } 1>&2; exit 2; fi; case ' $(APPROVE) ' in *' revision '*) \
	$(MKDIR_P) 'no-dist/.archived'; mv '$(rvdir)/$(nonportfile)' \
	'$@'; rmdir '$(rvdir)' 2> /dev/null || :; echo \
	'User'\''s revision has been accepted. To undo this, launch `$(MAKE) reject-revision`.'; \
	;; *) { echo; echo \
	'There are pending revisions. Please revise the content of the `$(rvdir)`'; \
	echo \
	'subdirectory, then launch `$(MAKE) approve-revision` after you are done.'; \
	echo; } >&2; exit 13; ;; esac;

 # Non-phony special prerequisite for compiling the package when one or more
 # standard data types are not available
 no-dist/hackings/baremetal/c-standard-library.h: no-dist/.archived/$(nonportfile)
	@echo 'Creating $@...' && $(MKDIR_P) 'no-dist/hackings/baremetal' && \
	{ ! test -f 'no-dist/conf-results/noioheaders.h' || cat \
	'no-dist/conf-results/noioheaders.h'; cat \
	'no-dist/.archived/$(nonportfile)'; } > '$@';

 # Defaults to `make REVISEME/non-portable.h`
 $(rvdir)/$(nonportfile): no-dist/conf-results/stdmissing.h no-dist/.archived/$(nonportfile)
	@$(MKDIR_P) '$(rvdir)'; test -f 'no-dist/.archived/$(nonportfile)' && \
	mv 'no-dist/.archived/$(nonportfile)' '$@' || cp \
	'no-dist/conf-results/stdmissing.h' '$@'; echo \
	'User'\''s revision has been rejected.';

 # Phony special targets

 .PHONY: approve-revision
 approve-revision:
	@$(MAKE) APPROVE='revision' \
	'no-dist/hackings/baremetal/c-standard-library.h' \
	baremetal-source-code;

 .PHONY: reject-revision
 reject-revision: $(rvdir)/$(nonportfile)

endif REVISE_LIBC


###############################################################################
##    R E C I P E S   P A R T   I I I:   A N A L Y S I S
###############################################################################

# Various `sandbox-*` phony targets... Each of them (except the `*check` ones)
# must erase and rebuild the `volatile` directory (or an alternative directory
# if a `SANDBOX` variable is passed to `make` - as in `make SANDBOX='man-pkg'
# sandbox-install-man`).

# For internal use they serve several purposes, while for public use they are
# mostly sugar targets for invoking `make install*` with a relative path as
# `DESTDIR` (by passing a `SANDBOX` variable), or unpacking the various
# distribution targets into a volatile directory.

.PHONY: sandbox-void
sandbox-void: ;

.PHONY: sandbox-erase
sandbox-erase:
	@rm -rf '$(SANDBOX)';

.PHONY: sandbox-blank
sandbox-blank: sandbox-erase
	@$(MKDIR_P) '$(SANDBOX)';

.PHONY: sandbox-beforedone
sandbox-beforedone: $(SANDBOX)
	@_FULLPATH="$$(cd '$(SANDBOX)' && pwd)" && $(MAKE) \
	DESTDIR="$${_FULLPATH}" $(FINALJOBS);

.PHONY: sandbox-installcheck
sandbox-installcheck: $(SANDBOX)
	@$(MAKE) FINALJOBS='installcheck' sandbox-beforedone;

.PHONY: sandbox-install
sandbox-install: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install' sandbox-beforedone;

.PHONY: sandbox-install-strip
sandbox-install-strip: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install-strip' sandbox-beforedone;

.PHONY: sandbox-install-exec
sandbox-install-exec: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install-exec' sandbox-beforedone;

.PHONY: sandbox-install-exec-strip
sandbox-install-exec-strip: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install-exec-strip' \
	sandbox-beforedone;

.PHONY: sandbox-install-headers
sandbox-install-headers: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install-headers' sandbox-beforedone;

.PHONY: sandbox-install-man
sandbox-install-man: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install-man' sandbox-beforedone;

.PHONY: sandbox-install-data
sandbox-install-data: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install-data' sandbox-beforedone;

.PHONY: sandbox-install-html
sandbox-install-html: sandbox-blank
	@$(MAKE) RUNJOB='void' FINALJOBS='install-html' sandbox-beforedone;

.PHONY: sandbox-binary-release sandbox-nearlydone
sandbox-binary-release sandbox-nearlydone: sandbox-$(INSTALLFLAVOR)

.PHONY: sandbox-alldone
sandbox-alldone: sandbox-$(INSTALLFLAVOR)
	@$(MAKE) sandbox-beforedone;

.PHONY: sandbox-portable-builds
sandbox-portable-builds: sandbox-blank
	@rm -rf '._excfiles_' && $(MAKE) SANDBOX='._excfiles_' RUNJOB='blank' \
	FINALJOBS='install-exec-strip install-headers' sandbox-beforedone && \
	$(FIND) -L '._excfiles_' -type f -exec cp -a {} '$(SANDBOX)/' \; && \
	cp '$(srcdir)/COPYING' '$(SANDBOX)/COPYING' && rm -rf '._excfiles_';

.PHONY: sandbox-dist sandbox-distdir
sandbox-dist sandbox-distdir: sandbox-erase
	@rm -rf '._dstfiles_' && $(MAKE) distdir='._dstfiles_' distdir && mv \
	'._dstfiles_' '$(SANDBOX)';

.PHONY: sandbox-source-release
sandbox-source-release: sandbox-dist
	@case ' $(DISTFLAGS) ' in *' no-symlinks '*) : ;; *) for _FILE in \
	$$(cd '$(SANDBOX)' && $(FIND) -L . ! -name '.'); do ! test -L \
	'$(srcdir)/'"$${_FILE}" || (rm -rf '$(SANDBOX)/'"$${_FILE}" && cp -d \
	'$(srcdir)/'"$${_FILE}" '$(SANDBOX)/'"$${_FILE}"); done ;; esac;

.PHONY: sandbox-authors-copy
sandbox-authors-copy: sandbox-source-release
	@(cd '$(SANDBOX)' && (rm -rf $(_bootstrapclean_dirs_); \
	rm -f $(_bootstrapclean_files_)));

.PHONY: sandbox-snapshot
sandbox-snapshot:
	@if test "x$$(pwd)" != "x$$(cd '$(srcdir)' && pwd)"; then { echo \
	'**Error** This operation is permitted only when building from the top level'; \
	echo 'directory.'; } 1>&2; exit 1; fi; rm -rf '$(SANDBOX)' && \
	$(MKDIR_P) '$(SANDBOX)' && $(FIND) -L '$(srcdir)' -maxdepth 1 ! -name \
	'.' ! -name '$(SANDBOX)' $$({ for _COIN in $(SNAPIGNORE); do echo "! \
	-name $${_COIN}"; done; }) -exec cp -at '$(SANDBOX)' {} \; && $(MAKE) \
	-C '$(SANDBOX)' BAKPATH='@PACKAGE_TARNAME@-snapshot' \
	SANDBOX='volatile' oblivion-clean;

.PHONY: sandbox-git-clone
sandbox-git-clone: sandbox-erase
	@git clone $(GITFLAGS) \
	'@GIT_HTTP_SERVER@/@PUBLISHER_ID@/@PACKAGE_TARNAME@.git' '$(SANDBOX)';

# Non-phony targets with names editable on the fly (mostly `Makefile` glue, but
# nothing forbids invoking them)

# Defaults to `make volatile` (directory)
$(SANDBOX):
	@$(MAKE) 'sandbox-$(RUNJOB)';

# Defaults to `make volatile.tar`
$(SANDBOX).tar: $(SANDBOX)
	@rm -rf '$@' && (cd '$(SANDBOX)' && $(AMTAR) -cf '../$(@F)' *);

# Defaults to `make volatile.tar.gz` and `make volatile.tgz`
$(SANDBOX).tar.gz $(SANDBOX).tgz: $(SANDBOX)
	@rm -rf '$@' && (cd '$(SANDBOX)' && $(AMTAR) -czf '../$(@F)' *);

# Defaults to `make volatile.tar.xz` and `make volatile.txz`
$(SANDBOX).tar.xz $(SANDBOX).txz: $(SANDBOX)
	@rm -rf '$@' && (cd '$(SANDBOX)' && $(AMTAR) -cJf '../$(@F)' *);

# Defaults to `make volatile.zip`
$(SANDBOX).zip: $(SANDBOX)
	@rm -rf '$@' && (cd '$(SANDBOX)' && zip -rqy '../$(@F)' *);

# Defaults to `make volatile.lst` (optionally supports a `LISTPREFIX` variable)
$(SANDBOX).lst: $(SANDBOX)
	@rm -rf '$@' && (cd '$(SANDBOX)' && $(FIND) -L . -type f -printf \
	'$(LISTPREFIX)%P\n' > '../$(@F)');


###############################################################################
##    R E C I P E S   P A R T   I V:   D I S T R I B U T I O N
###############################################################################

# Non-phony targets with names editable on the fly (mostly `Makefile` glue, but
# nothing forbids invoking them)

# Defaults to `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-pkg`
# (directory)
$(BINPATH):
	@$(MAKE) RUNJOB='binary-release' SANDBOX='._binfiles_' \
	'._binfiles_' && mv '._binfiles_' '$@';

# Defaults to `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-builds`
# (directory)
$(PTBPATH):
	@$(MAKE) RUNJOB='portable-builds' SANDBOX='._ptbfiles_' \
	'._ptbfiles_' && mv '._ptbfiles_' '$@';

# Defaults to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-src` (directory)
$(SRCPATH):
	@$(MAKE) RUNJOB='source-release' SANDBOX='._srcfiles_' \
	'._srcfiles_' && mv '._srcfiles_' '$@';

# Defaults to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-dev` (directory)
$(DEVPATH):
	@$(MAKE) RUNJOB='authors-copy' SANDBOX='._devfiles_' \
	'._devfiles_' && mv '._devfiles_' '$@';

$(BAKPATH):
	@$(MAKE) RUNJOB='snapshot' SANDBOX='._bakfiles_' \
	'._bakfiles_' && mv '._bakfiles_' '$@';

# Defaults to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-git` (directory)
$(GITPATH):
	@$(MAKE) RUNJOB='git-clone' SANDBOX='._gitfiles_' '._gitfiles_' && \
	mv '._gitfiles_' '$@';

# Default to
# `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-pkg.lst` and
# `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-pkg.{...EXT...}`
$(BINPATH).lst $(BINPATH).tar $(BINPATH).tar.gz $(BINPATH).tgz $(BINPATH).tar.xz $(BINPATH).txz $(BINPATH).zip:
	@_STEM="$$(basename '$(BINPATH)')" && $(MAKE) \
	RUNJOB='binary-release' SANDBOX="._tmp-$${_STEM}" '._tmp-$(@F)' && \
	mv '._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to
# `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-builds.{...EXT...}`
$(PTBPATH).tar $(PTBPATH).tar.gz $(PTBPATH).tgz $(PTBPATH).tar.xz $(PTBPATH).txz $(PTBPATH).zip:
	@_STEM="$$(basename '$(PTBPATH)')" && $(MAKE) \
	RUNJOB='portable-builds' SANDBOX="._tmp-$${_STEM}" '._tmp-$(@F)' && \
	mv '._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-src.{...EXT...}`
$(SRCPATH).tar $(SRCPATH).tar.gz $(SRCPATH).tgz $(SRCPATH).tar.xz $(SRCPATH).txz $(SRCPATH).zip:
	@_STEM="$$(basename '$(SRCPATH)')" && $(MKDIR_P) "._tmp-$${_STEM}" && \
	$(MAKE) SANDBOX="._tmp-$${_STEM}/$${_STEM}" sandbox-source-release && \
	$(MAKE) RUNJOB='void' SANDBOX="._tmp-$${_STEM}" '._tmp-$(@F)' && mv \
	'._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-dev.{...EXT...}`
$(DEVPATH).tar $(DEVPATH).tar.gz $(DEVPATH).tgz $(DEVPATH).tar.xz $(DEVPATH).txz $(DEVPATH).zip:
	@_STEM="$$(basename '$(DEVPATH)')" && $(MKDIR_P) "._tmp-$${_STEM}" && \
	$(MAKE) SANDBOX="._tmp-$${_STEM}/$${_STEM}" sandbox-authors-copy && \
	$(MAKE) RUNJOB='void' SANDBOX="._tmp-$${_STEM}" '._tmp-$(@F)' && mv \
	'._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-src.{...EXT...}`
$(BAKPATH).tar $(BAKPATH).tar.gz $(BAKPATH).tgz $(BAKPATH).tar.xz $(BAKPATH).txz $(BAKPATH).zip:
	@_STEM="$$(basename '$(BAKPATH)')" && $(MAKE) SANDBOX="._bakfiles_" \
	sandbox-snapshot && $(MKDIR_P) "._tmp-$${_STEM}" && mv '._bakfiles_' \
	"._tmp-$${_STEM}/$${_STEM}" && $(MAKE) RUNJOB='void' \
	SANDBOX="._tmp-$${_STEM}" '._tmp-$(@F)' && mv '._tmp-$(@F)' '$@' && \
	rm -rf "._tmp-$${_STEM}";

# Default to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-git.{...EXT...}`
$(GITPATH).tar $(GITPATH).tar.gz $(GITPATH).tgz $(GITPATH).tar.xz $(GITPATH).txz $(GITPATH).zip:
	@_STEM="$$(basename '$(GITPATH)')" && $(MKDIR_P) "._tmp-$${_STEM}" && \
	$(MAKE) SANDBOX="._tmp-$${_STEM}/$${_STEM}" sandbox-git-clone && \
	$(MAKE) RUNJOB='void' SANDBOX="._tmp-$${_STEM}" '._tmp-$(@F)' && mv \
	'._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";


# Phony targets

# File extensions for which archive targets exist (regular expression)
_re_archive_allow_ = \.tar$$\|\.tar\.gz$$\|\.tar\.xz$$\|\.tgz$$\|\.txz$$\|\.zip$$

# Build a source release that needs Autotools and a `bootstrap` script
.PHONY: authors-copy
authors-copy:
	@if test 'x$(DEST)' != x; then rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'DEVPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo DEVPATH='$(DEST)')" '$(DEST)'; else \
	rm -rf '$(DEVPATH).$(ZIPTYPE)' && $(MAKE) '$(DEVPATH).$(ZIPTYPE)'; fi;

# Build an installation image ready to be distributed or installed
.PHONY: binary-release
binary-release:
	@if test 'x$(DEST)' != x; then rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'BINPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo BINPATH='$(DEST)')" '$(DEST)'; else \
	rm -rf '$(BINPATH).$(ZIPTYPE)' && $(MAKE) '$(BINPATH).$(ZIPTYPE)'; fi;

# Clone the latest revision from the git server (requires `git`)
.PHONY: git-clone
git-clone:
	@if test 'x$(DEST)' != x; then rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'GITPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo GITPATH='$(DEST)')" '$(DEST)'; else \
	rm -rf '$(GITPATH).$(ZIPTYPE)' && $(MAKE) '$(GITPATH).$(ZIPTYPE)'; fi;

# Create a list of the files that will be installed by `make-install`
.PHONY: manifest
manifest:
	@rm -rf '._tmp_.lst'; $(MAKE) RUNJOB='install' SANDBOX='._tmp_' \
	LISTPREFIX='/' '._tmp_.lst' && (test 'x$(DEST)' != x && (rm -rf \
	'$(DEST)' && mv '._tmp_.lst' '$(DEST)') || (rm -rf '$(BINPATH).lst' \
	&& mv '._tmp_.lst' '$(BINPATH).lst')) && rm -rf '._tmp_';

# Build a package with only the compiled binaries and without directories
.PHONY: portable-builds
portable-builds:
	@if test 'x$(DEST)' != x; then rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'PTBPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo PTBPATH='$(DEST)')" '$(DEST)'; else \
	rm -rf '$(PTBPATH).$(ZIPTYPE)' && $(MAKE) '$(PTBPATH).$(ZIPTYPE)'; fi;

# Build a source release ready to be distributed
.PHONY: source-release
source-release:
	@if test 'x$(DEST)' != x; then rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'SRCPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo SRCPATH='$(DEST)')" '$(DEST)'; else \
	rm -rf '$(SRCPATH).$(ZIPTYPE)' && $(MAKE) '$(SRCPATH).$(ZIPTYPE)'; fi;

# Create a clean backup of **everything**, including non-distributable files
.PHONY: snapshot
snapshot:
	@if test 'x$(DEST)' != x; then rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'BAKPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo BAKPATH='$(DEST)')" '$(DEST)'; else \
	_STAMP="$$(date '+%Y-%m-%d-%H-%M-%S')" && rm -rf \
	'$(BAKPATH)-'"$${_STAMP}"'.$(ZIPTYPE)' && $(MAKE) \
	BAKPATH='$(BAKPATH)-'"$${_STAMP}" \
	'$(BAKPATH)-'"$${_STAMP}"'.$(ZIPTYPE)'; fi;


## EOF

