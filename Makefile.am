## Process this file with automake to produce Makefile.in


###############################################################################
##    V A R I A B L E S   A N D   G E N E R A L   S E T T I N G S
###############################################################################

# NOTE:  Upper case variables may be freely overridden by the user, while lower
#        case variables, especially those beginning with an underscore, are for
#        internal use only.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	src

EXTRA_DIST = \
	ChangeLog.md \
	MANUAL.md \
	README.md \
	autostuff \
	bootstrap \
	docs \
	mgwmake.bat \
	package.json \
	tests \
	.editorconfig

dist_doc_DATA = \
	AUTHORS \
	COPYING


if WANT_DEVEL

# Create `$(PROJECT_CONFNAME)-$(PROJECT_EFFVER).h` and
# `$(PROJECT_CONFNAME)-$(PROJECT_MAJVER).h`
_header_insthook_1_ = \
	mv '$(DESTDIR)$(includedir)/@PROJECT_NAME@.h' \
	'$(DESTDIR)$(includedir)/@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' && \
	(cd '$(DESTDIR)$(includedir)' && \
	$(LN_S) '@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' \
	'@PROJECT_CONFNAME@-@PROJECT_MAJVER@.h');


if !MULTIVERSION

# Create `$(PROJECT_CONFNAME).h` only when the package must *not* coexist with
# other major versions of itself
_header_insthook_2_ = \
	(cd '$(DESTDIR)$(includedir)' && \
	$(LN_S) '@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' \
	'@PROJECT_CONFNAME@.h');

endif !MULTIVERSION


_header_uninsthook_ = \
	-rm -f '$(DESTDIR)$(includedir)/@PROJECT_CONFNAME@-@PROJECT_EFFVER@.h' \
	'$(DESTDIR)$(includedir)/@PROJECT_CONFNAME@-@PROJECT_MAJVER@.h';


if WANT_DOC

dist_doc_DATA += \
	ChangeLog \
	NEWS \
	README \
	docs/manual.html


# Currently it is not possible to do `notrans_man_MANS = $(MANPAGE_LIST)`,
# because although the variable gets correctly assigned, it results still empty
# when Automake creates the rules, so no rules for man pages will be created. A
# workaround is to make Automake see at least one random file, and then re-set
# the variable properly immediately afterwards, in a way that Automake cannot
# see.

notrans_man_MANS = \
	docs/man/man3/libconfini.3

@POPULATE_MANPAGE_LIST@

endif WANT_DOC


if WANT_EXAMPLES

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

examplesmiscdir = $(examplesdir)/miscellanea
dist_examplesmisc_DATA = \
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
# itself or if it has been renamed
_examples_insthook_1_ = \
	$(SED) -i 's/-l@PROJECT_NAME@/-l@PROJECT_LOCALNAME@/g' \
	'$(DESTDIR)$(examplesdir)/run_example.sh' && \
	$(FIND) '$(DESTDIR)$(examplesdir)' -type f -name '*.c' | $(XARGS) \
	$(SED) -i 's/<@PROJECT_NAME@.h>/<@library_headername@.h>/g';

endif HAVE_TRANSFORMATIONS


# Make examples script executable
_examples_insthook_2_ = \
	chmod +x '$(DESTDIR)$(examplesdir)/run_example.sh';

endif WANT_EXAMPLES


endif WANT_DEVEL


# Files/directories erased by `make clean`

RUNPATH = volatile


# Files/directories erased by `make oblivion-clean`

BINPATH = @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-pkg

PTBPATH = @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-builds

SRCPATH = @PACKAGE_TARNAME@-@PACKAGE_VERSION@-src

DEVPATH = @PACKAGE_TARNAME@-@PACKAGE_VERSION@-dev

GITPATH = @PACKAGE_TARNAME@-git


# Other settings

# Default compression format
ZIPTYPE = tar.gz

# Shell expansion is supported here
_authorclean_files_ = \
	'aclocal.m4' \
	'config'.* \
	'configure' \
	'libtool' \
	'ltmain.sh' \
	`$(FIND) . -type f -name 'Makefile.in'`

# Shell expansion is supported here
_authorclean_dirs_ = \
	'build-aux' \
	'autom4te.cache' \
	'm4' \
	`$(FIND) . -type d -name .deps`


###############################################################################
##    R E C I P E S   P A R T   I :   D E V E L O P M E N T   A N D   U S E
###############################################################################

# Supplementary targets for GNU Automake

install-data-hook:
	$(_header_insthook_1_)
	$(_header_insthook_2_)
	$(_examples_insthook_1_)
	$(_examples_insthook_2_)

uninstall-local:
	$(_header_uninsthook_)
	-rm -rf '$(DESTDIR)$(docdir)';

clean-local:
	@renaming_clean_cmds@
	-rm -rf '$(RUNPATH)' '._'*;
	-rm -f '$(RUNPATH).lst' '$(RUNPATH).tar.gz' \
	'$(RUNPATH).tgz' '$(RUNPATH).tar.xz' '$(RUNPATH).txz' \
	'$(RUNPATH).zip';

distclean-local:
	@renaming_distclean_cmds@
	-rm -rf 'no-dist' '@confnewdir@';
	-rm -f 'make-targets.txt';

dist-hook:
	@renaming_dist_cmds@


# Novel targets

.PHONY: install-exec-strip
install-exec-strip:
	test -z '$(STRIP)' && $(MAKE) $(AM_MAKEFLAGS) \
	INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	install-exec || $(MAKE) $(AM_MAKEFLAGS) \
	INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	"INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'" install-exec;

# Make the source directory depend on Autotools and a `bootstrap` script
.PHONY: author-clean
author-clean: maintainer-clean
	rm -rf $(_authorclean_dirs_) && rm -f $(_authorclean_files_) && \
	(cd '$(srcdir)' && rm -rf $(_authorclean_dirs_) && \
	rm -f $(_authorclean_files_));

# Merge the files created by `./configure --enable-extended-config=sandbox`
.PHONY: synch
synch:
	$(AM_V_GEN)@synch_cmds@

# Uninstall what `make install` has created, then clean the source directory
.PHONY: clobber
clobber: uninstall clean

# Print the list of common targets
.PHONY: help
help:
	$(AM_V_GEN)echo && cat 'make-targets.txt' && \
	echo 'This package is currently configured for a @host_cpu@ @host_vendor@ running @host_os@.' || \
	echo 'Please launch `$(srcdir)/configure` again.';


###############################################################################
##    R E C I P E S   P A R T   I I:   A N A L Y S I S
###############################################################################

# Various `dryrun-*` phony targets... Each of them must erase and rebuild the
# `volatile` directory (or an alternative directory if a `RUNPATH` variable
# is passed to `make` - as in `make RUNPATH='man-pkg' dryrun-install-man`).

# For internal use they serve several purposes, while for public use they are
# mostly sugar targets for invoking `make install*` with a relative path as
# `DESTDIR` (by passing a `RUNPATH` variable), or unpacking the various
# distribution targets into a volatile directory.

.PHONY: dryrun-install
dryrun-install:
	rm -rf '$(RUNPATH)' && $(MKDIR_P) '$(RUNPATH)' && \
	_FULLPATH="$$(cd '$(RUNPATH)' && pwd)" && \
	$(MAKE) DESTDIR="$${_FULLPATH}" install$(_install_suffix_);

.PHONY: dryrun-install-strip dryrun-binary-release
dryrun-install-strip dryrun-binary-release:
	@$(MAKE) _install_suffix_='-strip' dryrun-install;

.PHONY: dryrun-install-exec
dryrun-install-exec:
	@$(MAKE) _install_suffix_='-exec' dryrun-install;

.PHONY: dryrun-install-exec-strip
dryrun-install-exec-strip:
	@$(MAKE) _install_suffix_='-exec-strip' dryrun-install;

.PHONY: dryrun-install-man
dryrun-install-man:
	@$(MAKE) _install_suffix_='-man' dryrun-install;

.PHONY: dryrun-install-data
dryrun-install-data:
	@$(MAKE) _install_suffix_='-data' dryrun-install;

.PHONY: dryrun-portable-builds
dryrun-portable-builds:
	$(MAKE) RUNPATH='._excfiles_' dryrun-install-exec-strip && \
	rm -rf '$(RUNPATH)' && $(MKDIR_P) '$(RUNPATH)' && \
	$(FIND) -L '._excfiles_' -type f | $(XARGS) -I _files_ mv _files_ \
	'$(RUNPATH)/' && cp '$(srcdir)/COPYING' '$(RUNPATH)/COPYING' && \
	cp '$(srcdir)/src/@PROJECT_NAME@.h' '$(RUNPATH)/@PROJECT_NAME@.h' && \
	rm -rf '._excfiles_';

.PHONY: dryrun-dist
dryrun-dist:
	rm -rf '$(RUNPATH)' '._dstfiles_' && $(MAKE) distdir='._dstfiles_' \
	distdir && mv '._dstfiles_' '$(RUNPATH)';

.PHONY: dryrun-source-release
dryrun-source-release: dryrun-dist
	(cd '$(RUNPATH)' && rm -f 'ChangeLog.md' 'README.md' && \
	$(LN_S) 'ChangeLog' 'ChangeLog.md' && $(LN_S) 'README' 'README.md');

.PHONY: dryrun-authors-copy
dryrun-authors-copy: dryrun-source-release
	(cd '$(RUNPATH)' && rm -rf $(_authorclean_dirs_) && \
	rm -f $(_authorclean_files_));

.PHONY: dryrun-git-clone
dryrun-git-clone:
	@rm -rf '$(RUNPATH)' && git clone --quiet --progress \
	'@GIT_HTTP_SERVER@/@PUBLISHER_ID@/@PACKAGE_TARNAME@.git' '$(RUNPATH)';

.PHONY: dryrun-nothing
dryrun-nothing: ;

# Non-phony targets with names editable on the fly (mostly for internal use,
# but nothing forbids invoking them)

# Default `dryrun-*` target for `make volatile*`
RUNJOB = binary-release

# Defaults to `make volatile` (directory)
$(RUNPATH): dryrun-$(RUNJOB)

# Defaults to `make volatile.tar.xz` and `make volatile.txz`
$(RUNPATH).tar.xz $(RUNPATH).txz: $(RUNPATH)
	rm -rf '$@' && (cd '$(RUNPATH)' && $(AMTAR) -cJf '../$(@F)' *);

# Defaults to `make volatile.tar.gz` and `make volatile.tgz`
$(RUNPATH).tar.gz $(RUNPATH).tgz: $(RUNPATH)
	rm -rf '$@' && (cd '$(RUNPATH)' && $(AMTAR) -czf '../$(@F)' *);

# Defaults to `make volatile.zip`
$(RUNPATH).zip: $(RUNPATH)
	rm -rf '$@' && (cd '$(RUNPATH)' && zip -rqy '../$(@F)' *);

# Defaults to `make volatile.lst` (optionally supports a `LISTPREFIX` variable)
$(RUNPATH).lst: $(RUNPATH)
	rm -rf '$@' && (cd '$(RUNPATH)' && $(FIND) -L * -type f -printf \
	'$(LISTPREFIX)%p\n' > '../$(@F)');


###############################################################################
##    R E C I P E S   P A R T   I I I:   D I S T R I B U T I O N
###############################################################################

# Non-phony targets with names editable on the fly (mostly for internal use,
# but nothing forbids invoking them)

# Defaults to `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-pkg`
# (directory)
$(BINPATH):
	@$(MAKE) RUNJOB='binary-release' RUNPATH='._binfiles_' \
	'._binfiles_' && mv '._binfiles_' '$@';

# Defaults to `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-builds`
# (directory)
$(PTBPATH):
	@$(MAKE) RUNJOB='portable-builds' RUNPATH='._ptbfiles_' \
	'._ptbfiles_' && mv '._ptbfiles_' '$@';

# Defaults to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-src` (directory)
$(SRCPATH):
	@$(MAKE) RUNJOB='source-release' RUNPATH='._srcfiles_' \
	'._srcfiles_' && mv '._srcfiles_' '$@';

# Defaults to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-dev` (directory)
$(DEVPATH):
	@$(MAKE) RUNJOB='authors-copy' RUNPATH='._devfiles_' \
	'._devfiles_' && mv '._devfiles_' '$@';

# Defaults to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-git` (directory)
$(GITPATH):
	@$(MAKE) RUNJOB='git-clone' RUNPATH='._gitfiles_' '._gitfiles_' && \
	mv '._gitfiles_' '$@';

# Default to
# `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-pkg.lst` and
# `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-pkg.{...EXT...}`
$(BINPATH).lst $(BINPATH).tar.gz $(BINPATH).tgz $(BINPATH).tar.xz $(BINPATH).txz $(BINPATH).zip:
	@_STEM="$$(basename '$(BINPATH)')" && $(MAKE) \
	RUNJOB='binary-release' RUNPATH="._tmp-$${_STEM}" '._tmp-$(@F)' && \
	mv '._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to
# `make @PACKAGE_LOCALNAME@-@PACKAGE_VERSION@-@host@-builds.{...EXT...}`
$(PTBPATH).tar.gz $(PTBPATH).tgz $(PTBPATH).tar.xz $(PTBPATH).txz $(PTBPATH).zip:
	@_STEM="$$(basename '$(PTBPATH)')" && $(MAKE) \
	RUNJOB='portable-builds' RUNPATH="._tmp-$${_STEM}" '._tmp-$(@F)' && \
	mv '._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-src.{...EXT...}`
$(SRCPATH).tar.gz $(SRCPATH).tgz $(SRCPATH).tar.xz $(SRCPATH).txz $(SRCPATH).zip:
	@_STEM="$$(basename '$(SRCPATH)')" && $(MKDIR_P) "._tmp-$${_STEM}" && \
	$(MAKE) RUNPATH="._tmp-$${_STEM}/$${_STEM}" dryrun-source-release && \
	$(MAKE) RUNJOB='nothing' RUNPATH="._tmp-$${_STEM}" '._tmp-$(@F)' && \
	mv '._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-dev.{...EXT...}`
$(DEVPATH).tar.gz $(DEVPATH).tgz $(DEVPATH).tar.xz $(DEVPATH).txz $(DEVPATH).zip:
	@_STEM="$$(basename '$(DEVPATH)')" && $(MKDIR_P) "._tmp-$${_STEM}" && \
	$(MAKE) RUNPATH="._tmp-$${_STEM}/$${_STEM}" dryrun-authors-copy && \
	$(MAKE) RUNJOB='nothing' RUNPATH="._tmp-$${_STEM}" '._tmp-$(@F)' && \
	mv '._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";

# Default to `make @PACKAGE_TARNAME@-@PACKAGE_VERSION@-git.{...EXT...}`
$(GITPATH).tar.gz $(GITPATH).tgz $(GITPATH).tar.xz $(GITPATH).txz $(GITPATH).zip:
	@_STEM="$$(basename '$(GITPATH)')" && $(MKDIR_P) "._tmp-$${_STEM}" && \
	$(MAKE) RUNPATH="._tmp-$${_STEM}/$${_STEM}" dryrun-git-clone && \
	$(MAKE) RUNJOB='nothing' RUNPATH="._tmp-$${_STEM}" '._tmp-$(@F)' && \
	mv '._tmp-$(@F)' '$@' && rm -rf "._tmp-$${_STEM}";


# Phony targets

# File extensions for which archive targets exist (regular expression)
_re_archive_allow_ = \.tar\.gz$$\|\.tar\.xz$$\|\.tgz$$\|\.txz$$\|\.zip$$

# Build a source release that needs Autotools and a `bootstrap` script
.PHONY: authors-copy
authors-copy:
	@test 'x$(DEST)' != x && (rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'DEVPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo DEVPATH='$(DEST)')" '$(DEST)') || \
	(rm -rf '$(DEVPATH).$(ZIPTYPE)' && $(MAKE) '$(DEVPATH).$(ZIPTYPE)');

# Build an installation image ready to be distributed or installed
.PHONY: binary-release
binary-release:
	@test 'x$(DEST)' != x && (rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'BINPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo BINPATH='$(DEST)')" '$(DEST)') || \
	(rm -rf '$(BINPATH).$(ZIPTYPE)' && $(MAKE) '$(BINPATH).$(ZIPTYPE)');

# Clone the latest revision from the Git server (requires `git`)
.PHONY: git-clone
git-clone:
	@test 'x$(DEST)' != x && (rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'GITPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo GITPATH='$(DEST)')" '$(DEST)') || \
	(rm -rf '$(GITPATH).$(ZIPTYPE)' && $(MAKE) '$(GITPATH).$(ZIPTYPE)');

# Create a list of the files that will be installed by `make-install`
.PHONY: manifest
manifest:
	@rm -rf '._tmp_.lst'; $(MAKE) RUNJOB='install' RUNPATH='._tmp_' \
	LISTPREFIX='/' '._tmp_.lst' && (test 'x$(DEST)' != x && (rm -rf \
	'$(DEST)' && mv '._tmp_.lst' '$(DEST)') || (rm -rf '$(BINPATH).lst' \
	&& mv '._tmp_.lst' '$(BINPATH).lst')) && rm -rf '._tmp_';

# Build a package with only the compiled binaries and without directories
.PHONY: portable-builds
portable-builds:
	@test 'x$(DEST)' != x && (rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'PTBPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo PTBPATH='$(DEST)')" '$(DEST)') || \
	(rm -rf '$(PTBPATH).$(ZIPTYPE)' && $(MAKE) '$(PTBPATH).$(ZIPTYPE)');

# Build a source release ready to be distributed
.PHONY: source-release
source-release:
	@test 'x$(DEST)' != x && (rm -rf '$(DEST)' && $(MAKE) "$$(expr \
	'SRCPATH=$(DEST)' : '\(.*\)\($(_re_archive_allow_)\)' || \
	echo SRCPATH='$(DEST)')" '$(DEST)') || \
	(rm -rf '$(SRCPATH).$(ZIPTYPE)' && $(MAKE) '$(SRCPATH).$(ZIPTYPE)');

# Call `make author-clean`, then remove all the releases created
.PHONY: oblivion-clean
oblivion-clean: author-clean
	rm -rf '$(BINPATH)' '$(PTBPATH)' '$(SRCPATH)' '$(DEVPATH)' \
	'$(GITPATH)' && rm -f '$(BINPATH).lst' '$(BINPATH).tar.gz' \
	'$(BINPATH).tar.xz' '$(BINPATH).tgz' '$(BINPATH).txz' \
	'$(BINPATH).zip' '$(PTBPATH).tar.gz' '$(PTBPATH).tar.xz' \
	'$(PTBPATH).tgz' '$(PTBPATH).txz' '$(PTBPATH).zip' \
	'$(SRCPATH).tar.gz' '$(SRCPATH).tar.xz' '$(SRCPATH).tgz' \
	'$(SRCPATH).txz' '$(SRCPATH).zip' '$(DEVPATH).tar.gz' \
	'$(DEVPATH).tar.xz' '$(DEVPATH).tgz' '$(DEVPATH).txz' \
	'$(DEVPATH).zip' '$(GITPATH).tar.gz' '$(GITPATH).tar.xz' \
	'$(GITPATH).tgz' '$(GITPATH).txz' '$(GITPATH).zip';


## EOF

