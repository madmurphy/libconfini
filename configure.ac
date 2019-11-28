##  -*- Mode: M4; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-
##  Process this file with autoconf to produce a configure script.

dnl
dnl        /|/|
dnl       (_|_)      _ _ _                      __ _       _
dnl                 | (_) |__   ___ ___  _ __  / _(_)_ __ (_)
dnl                 | | | '_ \ / __/ _ \| '_ \| |_| | '_ \| |
dnl                 | | | |_) | (_| (_) | | | |  _| | | | | |
dnl                 |_|_|_.__/ \___\___/|_| |_|_| |_|_| |_|_|      _ _
dnl                                                               ( | )
dnl                                                               |/|/
dnl

m4_include([autostuff/m4/not-autotools.m4])

dnl  This package has not been tested with previous versions of GNU Autoconf...
AC_PREREQ([2.68])


dnl  **************************************************************************
dnl     P A C K A G E   M E T A D A T A
dnl  **************************************************************************

AC_INIT([lib]n4_mem([GL_PROJECT_NAME], [confini]),
	n4_mem([GL_PROJECT_MAJVER], [1])[.]n4_mem([GL_PROJECT_MINVER],
		[12])[.]n4_mem([GL_PROJECT_REVVER], [0]),
	[madmurphy333@gmail.com],
	[lib]GL_PROJECT_NAME,
	[https://madmurphy.github.io/libconfini])

NC_SET_GLOBALLY(
	[INTERFACE_NUMBER],			[3],
	[IMPLEMENTATION_NUMBER],	[0],
	[INTERFACE_COMPATIBILITY],	[3],
	[PROJECT_DESCRIPTION],		[Yet another INI parser],
	[LICENSE_NAME],				[GNU General Public License, version 3 or any
								later version],
	[LICENSE_ID],				[GPL-3.0-or-later],
	[PUBLISHER_NAME],			[Stefano Gioffré],
	[PUBLISHER_ID],				[madmurphy],
	[GIT_HTTP_SERVER],			[https://github.com]
)


dnl  **************************************************************************
dnl     R E N A M I N G   R U L E S
dnl  **************************************************************************

AC_SUBST([PROJECT_NAME], [GL_PROJECT_NAME])

AS_VAR_SET([na_tmppref],
	["$(test "x${program_prefix}" = xNONE || echo "${program_prefix}" \
		| sed 's/@<:@^@<:@:alnum:@:>@@:>@\+/-/g;s/@<:@A-Z@:>@/\L&/g')"])

AS_VAR_SET([na_tmpsuff],
	["$(test "x${program_suffix}" = xNONE || echo "${program_suffix}" \
		| sed 's/@<:@^@<:@:alnum:@:>@@:>@\+/-/g;s/@<:@A-Z@:>@/\L&/g')"])

AS_VAR_COPY([na_tmptransf], [program_transform_name])

AC_SUBST([PROJECT_CONFNAME],
	["$(echo "${na_tmppref}]GL_PROJECT_NAME[${na_tmpsuff}" | \
		sed "${na_tmptransf}")"])

AC_SUBST([PACKAGE_CONFNAME], ["lib${PROJECT_CONFNAME}"])

AC_ARG_WITH([other_versions],
	[AS_HELP_STRING([--with-other-versions],
		[make this package able to cohabitate with other major versions of
		itself @<:@default=no@:>@])],
	[:],
	[AS_VAR_SET([with_other_versions], ['no'])])

AM_CONDITIONAL([RENAME_PACKAGE],
	[test "x${PROJECT_CONFNAME}" != 'x]GL_PROJECT_NAME['])

AM_CONDITIONAL([MULTIVERSION],
	[test "x${with_other_versions}" != xno])

AM_CONDITIONAL([HAVE_TRANSFORMATIONS],
	[test "x${with_other_versions}" != xno -o \
		"x${PROJECT_CONFNAME}" != 'x]GL_PROJECT_NAME['])

AM_COND_IF([MULTIVERSION], [
	AS_VAR_SET([na_tmpvsuff], [']GL_PROJECT_MAJVER['])
	AS_VAR_SET([na_tmphvsuff], ['-]GL_PROJECT_MAJVER['])
])

AM_COND_IF([RENAME_PACKAGE],
	[AC_MSG_NOTICE([renaming ]AC_PACKAGE_TARNAME[ to ${PACKAGE_CONFNAME}...])])

AC_SUBST([PROJECT_LOCALNAME], ["${PROJECT_CONFNAME}${na_tmpvsuff}"])

AC_SUBST([PACKAGE_LOCALNAME], ["${PACKAGE_CONFNAME}${na_tmpvsuff}"])

AC_SUBST([library_headername], ["${PROJECT_CONFNAME}${na_tmphvsuff}"])

NS_FOR([na_tmp_arg],
	[AS_CASE([${na_tmp_arg}], [--docdir=*],
		[AS_VAR_SET([na_auto_docdir], [no])[]NS_BREAK])])

AM_COND_IF([HAVE_TRANSFORMATIONS],
	[AS_IF([test "x${na_auto_docdir}" != xno],
		[AS_VAR_SET([docdir],
			['${datarootdir}/doc/'"${PACKAGE_LOCALNAME}"])])])

NS_UNSET([na_tmppref], [na_tmpsuff], [na_tmptransf], [na_tmpvsuff],
	[na_tmphvsuff], [na_tmp_arg], [na_auto_docdir])


dnl  Prevent any further transformations from Autoconf (unless
dnl  `AC_CANONICAL_TARGET` is invoked after these lines)

AS_VAR_SET([program_prefix], ['NONE'])

AS_VAR_SET([program_suffix], ['NONE'])

AS_VAR_SET([program_transform_name], ['s,x,x,'])


dnl  **************************************************************************
dnl     A U T O C O N F   E N V I R O N M E N T
dnl  **************************************************************************

dnl  Temporarily unset `CFLAGS` and let `AC_PROG_CC_C99` guess freely
NS_MOVEVAR([na_tmpusrcflags], [CFLAGS])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_AUX_DIR([build-aux])

dnl  Add a `--with-io-api=API` option to the `configure` script
NC_CONFIG_SHADOW_DIR([autostuff/shadows])

AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes])

AC_ARG_PROGRAM

AC_PROG_CC_C99

dnl  Save the guessed `CFLAGS` for `AM_CFLAGS` (see `src/Makefile.am`)
AC_SUBST([guessed_am_cflags], ["${CFLAGS}"])

dnl  Restore `CFLAGS` according to the user's will
NS_MOVEVAR([CFLAGS], [na_tmpusrcflags])

AC_PROG_LN_S

AC_PROG_SED

AC_CHECK_HEADERS([stdint.h stdlib.h])

AC_C_INLINE

AC_TYPE_INT8_T

AC_TYPE_SIZE_T

AC_TYPE_UINT16_T

AC_TYPE_UINT32_T

AC_TYPE_UINT8_T

AC_FUNC_MALLOC

AS_VAR_SET([na_tmp_ioflavor], ['check'])

dnl  Add a `--with-io-api=API` option to the `configure` script
AC_ARG_WITH([io_api],
	[AS_HELP_STRING([--with-io-api=API],
		[build this package with a specific I/O API; possible values for API
		are: "standard" for using the C Standard I/O API, "posix" for using the
		POSIX I/O API, or "check" for automatically selecting the best I/O API
		supported by this machine @<:@default=check@:>@])],
	[AS_IF([test "x${withval}" = x -o "x${with_io_api}" = xyes],
		[AC_MSG_ERROR([an API value must be specified for option --with-io-api=API])],
		[AS_IF([test "x${with_io_api}" = xno],
			[AC_MSG_WARN([unrecognized option: --without-io-api])],
			[AS_VAR_COPY([na_tmp_ioflavor], [withval])])])],
	[:])

AS_CASE([${na_tmp_ioflavor}],
	[standard], [],
	[posix], [
		NC_IF_HAVE_POSIX_C([], [
			NC_MSG_WARNBOX([The POSIX I/O API is not supported on this machine
			(--with-io-api=posix). The C Standard I/O API will be used
			instead.])
			AS_VAR_SET([na_tmp_ioflavor], ['standard'])
		])
	],
	[check], [
		NC_IF_HAVE_POSIX_C(
			[AS_VAR_SET([na_tmp_ioflavor], ['posix'])],
			[AS_VAR_SET([na_tmp_ioflavor], ['standard'])]
		)
	],
	[*], [
		AC_MSG_ERROR([unrecognized option: --with-io-api=${na_tmp_ioflavor}])
	])

AC_SUBST([c_source_io_flavor],
	["-DCONFINI_IO_FLAVOR=CONFINI_$(echo "${na_tmp_ioflavor}" | \
		tr '[a-z]' '[A-Z]')"])

AS_UNSET([na_tmp_ioflavor])

AC_ARG_VAR([RC], [Microsoft Windows resource compiler command])

AC_ARG_VAR([RCFLAGS], [Microsoft Windows resource compiler flags])

NC_REQ_PROGS(
	[find],		[Unix find utility],
	[xargs],	[Unix xargs utility]
)

AS_IF([test "x${pkgconfigdir}" = x],
	[AS_VAR_SET([pkgconfigdir], ['$(libdir)/pkgconfig'])])

dnl  Add a `--with-pkgconfigdir=API` option to the `configure` script
AC_ARG_WITH([pkgconfigdir],
	[AS_HELP_STRING([--with-pkgconfigdir=DIR],
		[pkg-config installation directory
		@<:@default=<libdir>/pkgconfig@:>@])],
	[AS_IF([test "x${withval}" = x -o "x${with_pkgconfigdir}" = xyes],
		[AC_MSG_ERROR([a DIR value must be specified for option --with-pkgconfigdir=DIR])],
		[AS_IF([test "x${with_pkgconfigdir}" = xno],
			[AC_MSG_WARN([unrecognized option: --without-pkgconfigdir])],
			[AS_VAR_COPY([pkgconfigdir], [withval])])])],
	[:])

AC_SUBST([pkgconfigdir])

dnl  Add `--enable-version-info` and `--disable-version-info` options to the
dnl  `configure` script
AC_ARG_ENABLE([version-info],
	[NA_HELP_STRINGS([[--enable-version-info], [--disable-version-info]],
		[enable/disable libtool versioning system
		@<:@default=SYSTEM-DEPENDENT@:>@])],
	[:],
	[AS_VAR_SET([enable_version_info], ['check'])])

dnl  Add a `--disable-devel` option to the `configure` script
AC_ARG_ENABLE([devel],
	[AS_HELP_STRING([--disable-devel],
		[don't install development files (headers, static libraries,
		documentation, examples, etc.)
		@<:@default=no@:>@])],
	[AS_VAR_SET_IF([enable_static],
		[AS_IF([test "x${enable_devel}" = xno -a "x${enable_static}" != xno], [
			AC_MSG_WARN([ignoring --enable-static @{:@--disable-devel@:}@])
			AS_VAR_SET([enable_static], ['no'])
		])])],
	[AS_VAR_SET([enable_devel], ['yes'])])

dnl  Add a `--disable-doc` option to the `configure` script
AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--disable-doc],
		[don't generate any documentation @<:@default=<disable devel>@:>@])],
	[AS_IF([test "x${enable_devel}" = xno -a "x${enable_doc}" != xno], [
		AC_MSG_WARN([ignoring --enable-doc @{:@--disable-devel@:}@])
		AS_VAR_SET([enable_doc], ['no'])
	])],
	[AS_VAR_COPY([enable_doc], [enable_devel])])

dnl  Add `--enable-examples` and `--disable-examples` options to the
dnl  `configure` script
AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples],
		[don't generate examples @<:@default=<disable doc>@:>@])],
	[AS_IF([test "x${enable_devel}" = xno -a "x${enable_examples}" != xno], [
		AC_MSG_WARN([ignoring --enable-examples @{:@--disable-devel@:}@])
		AS_VAR_SET([enable_examples], ['no'])
	])],
	[AS_VAR_COPY([enable_examples], [enable_doc])])

AM_CONDITIONAL([WANT_DEVEL], [test "x${enable_devel}" != xno])

AM_CONDITIONAL([WANT_DOC], [test "x${enable_doc}" != xno])

AM_CONDITIONAL([WANT_EXAMPLES], [test "x${enable_examples}" != xno])


dnl  **************************************************************************
dnl     P L A T F O R M   C H E C K S
dnl  **************************************************************************

AC_CANONICAL_HOST

AS_CASE([${host_os}],
	[*mingw*], [
		AS_VAR_SET([na_platform_mswin], ['yes'])
		AS_VAR_SET([na_native_mswin], ['yes'])
		AM_CONDITIONAL([SKIP_VERSIONING],
			[test "x${enable_version_info}" = xno -o \
				"x${enable_version_info}" = xcheck])
	],
	[pw32*|*cygwin*|*msys*], [
		AS_VAR_SET([na_platform_mswin], ['yes'])
		AS_VAR_SET([na_native_mswin], ['no'])
		AM_CONDITIONAL([SKIP_VERSIONING],
			[test "x${enable_version_info}" = xno])
	],
	[*], [
		AS_VAR_SET([na_platform_mswin], ['no'])
		AS_VAR_SET([na_native_mswin], ['no'])
		AM_CONDITIONAL([SKIP_VERSIONING],
			[test "x${enable_version_info}" = xno])
	])

AM_CONDITIONAL([PLATFORM_MSWIN], [test "x${na_platform_mswin}" = xyes])

AM_CONDITIONAL([NATIVE_MSWIN], [test "x${na_native_mswin}" = xyes])


dnl  **************************************************************************
dnl     L I B T O O L
dnl  **************************************************************************

LT_INIT([win32-dll])

AM_COND_IF([PLATFORM_MSWIN], [LT_PROG_RC])

AM_CONDITIONAL([HAVE_RC], [test "x${RC}" != x])


dnl  **************************************************************************
dnl     F I N E   T U N I N G
dnl  **************************************************************************

dnl  **************************************************************************
dnl     TODO:  For a perfect optimization it would be needed to test `-O2`
dnl            and `-O3` under more conditions...
dnl  **************************************************************************

dnl  Optimization `-O3` seems to be working better than `-O2` when compiling
dnl  under native Microsoft Windows, in all other cases use `-O2`. Users can
dnl  always override the default value by passing a `CFLAGS` variable.
AM_COND_IF([NATIVE_MSWIN],
	[AS_VAR_SET([default_optimization], [3])],
	[AS_VAR_SET([default_optimization], [2])])

AC_SUBST([default_optimization])


dnl  **************************************************************************
dnl     O U T P U T
dnl  **************************************************************************

AC_CONFIG_FILES([
	Makefile
	src/${PACKAGE_LOCALNAME}.pc:src/]AC_PACKAGE_TARNAME[.pc.in
	src/Makefile
])

NC_THREATEN_FILES([package.json], [src/winres.rc])

dnl  PREPARE_MANPAGES(page1, renamed-page1[, ... pageN, renamed-pageN])
dnl  **************************************************************************
dnl
dnl  Apply renamings (if any) to man pages and set the `notrans_man_MANS`
dnl  variable
dnl
AC_DEFUN_ONCE([PREPARE_MANPAGES], [
	AM_COND_IF([WANT_DOC], [
		AS_IF([test -d 'no-dist/man3'],
			[rm -r 'no-dist/man3'])
		AS_MKDIR_P([no-dist/man3])
		AM_COND_IF([HAVE_TRANSFORMATIONS], [
			m4_for([_iter_], [1], [$#], [2], [
				cp "${srcdir}/docs/man/man3/m4_normalize(m4_argn(_iter_, $@))" \
					"no-dist/man3/m4_normalize(m4_argn(m4_incr(_iter_), $@))";
			])
		], [
			m4_for([_iter_], [1], [$#], [2], [
				cp "${srcdir}/docs/man/man3/m4_normalize(m4_argn(_iter_, $@))" \
					'no-dist/man3/';
			])
		])
		dnl  See `Makefile.am` for this workaround
		AS_VAR_SET([POPULATE_MANPAGE_LIST], 
			["notrans_man_MANS = $(echo no-dist/man3/*)"])
	], [
		AS_VAR_SET([POPULATE_MANPAGE_LIST], ["notrans_man_MANS ="])
	])
	AC_SUBST([POPULATE_MANPAGE_LIST])
	AM_SUBST_NOTMAKE([POPULATE_MANPAGE_LIST])
])

dnl  Man pages go here, each page accompanied by a safe cohabitation renaming
dnl  and expected to be found in `${srcdir}/docs/man/man3`
PREPARE_MANPAGES(
	AC_PACKAGE_TARNAME[.3],	[${PACKAGE_LOCALNAME}.3],
	GL_PROJECT_NAME[.h.3],	[${library_headername}.h.3],
	[IniStatistics.3],		[${PACKAGE_LOCALNAME}-IniStatistics.3],
	[IniFormat.3],			[${PACKAGE_LOCALNAME}-IniFormat.3],
	[IniDispatch.3],		[${PACKAGE_LOCALNAME}-IniDispatch.3]
)

dnl  More renaming and extended config rules...
AM_COND_IF([RENAME_PACKAGE], [
	AM_COND_IF([HAVE_EXTENDED_CONFIG], [
		NC_MSG_WARNBOX([Extended configuration has been called on a renamed
			package. Changes will be reverted with `make *clean` and will not
			be re-distributed with `make dist`!])
		AS_VAR_SET([renaming_dist_cmds],
			m4_normalize([']m4_foreach([_iter_], [NC_THREATENED_LIST], [[
				! test -f '\''$(srcdir)/'"]_iter_["'.dist'\'' ||
					cp '\''$(srcdir)/'"]_iter_["'.dist'\''
					'\''$(distdir)/'"]_iter_["\'';
			]])[']))
		AM_COND_IF([HAVE_UPDATES], [
			AS_VAR_SET([synch_cmds],
				m4_normalize([']m4_foreach([_iter_], [NC_THREATENED_LIST], [[
					test -f '\''$(srcdir)/'"]_iter_["'.dist'\'' ||
						cp '\''$(srcdir)/'"]_iter_["\''
						'\''$(srcdir)/'"]_iter_["'.dist'\'';
					cp '\''$(confnewdir)/'"]_iter_["\''
						'\''$(srcdir)/'"]_iter_["\'' &&
						echo '\''File $(srcdir)/'"]_iter_["'
						successfully updated.'\'' || echo '\''File
						$(confnewdir)/'"]_iter_["' not found.'\'';
				]])[']))
			# The the `.dist` files have been generated by `make synch`, hence
			# `make clean` must restore them
			AS_VAR_SET([renaming_clean_cmds],
				m4_normalize([']m4_foreach([_iter_], [NC_THREATENED_LIST], [[
					! test -f '\''$(srcdir)/'"]_iter_["'.dist'\'' ||
						mv '\''$(srcdir)/'"]_iter_["'.dist'\''
						'\''$(srcdir)/'"]_iter_["\'';
				]])[']))
		], [
			m4_foreach([_iter_], [NC_THREATENED_LIST],
				[test -f "${srcdir}/]_iter_[.dist" || \
					cp "${srcdir}/]_iter_[" "${srcdir}/]_iter_[.dist";])
			AS_VAR_SET([synch_cmds],
				m4_normalize(['echo '\''Launch `$(srcdir)/configure @<:@...@:>@
					--enable-extended-config=sandbox` to use `make
					synch`.'\']))
			# The the `.dist` files have been generated by the `configure`
			# script, hence `make distclean` must restore them
			AS_VAR_SET([renaming_distclean_cmds],
				m4_normalize([']m4_foreach([_iter_], [NC_THREATENED_LIST], [[
					! test -f '\''$(srcdir)/'"]_iter_["'.dist'\'' ||
						mv '\''$(srcdir)/'"]_iter_["'.dist'\''
						'\''$(srcdir)/'"]_iter_["\'';
				]])[']))
		])
	])
], [
	AM_COND_IF([HAVE_UPDATES],
		[AS_VAR_SET([synch_cmds],
			m4_normalize([']m4_foreach([_iter_], [NC_THREATENED_LIST], [[
				test -f '\''$(confnewdir)/'"]_iter_["\'' &&
					cp '\''$(confnewdir)/'"]_iter_["\''
					'\''$(srcdir)/'"]_iter_["\'' &&
					echo '\''File $(srcdir)/'"]_iter_["' successfully
					updated.'\'' || echo '\''File $(confnewdir)/'"]_iter_["'
					not found.'\'';
			]])[']))],
		[AS_VAR_SET([synch_cmds],
				m4_normalize(['echo '\''Launch `$(srcdir)/configure @<:@...@:>@
					--enable-extended-config=sandbox` to use `make synch`.'\']))
	])
])

AC_SUBST([renaming_dist_cmds])

AC_SUBST([synch_cmds])

AC_SUBST([renaming_clean_cmds])

AC_SUBST([renaming_distclean_cmds])

AC_SUBST([PROJECT_MAJVER], [']GL_PROJECT_MAJVER['])

AC_SUBST([PROJECT_MINVER], [']GL_PROJECT_MINVER['])

AC_SUBST([PROJECT_REVVER], [']GL_PROJECT_REVVER['])

AC_SUBST([PROJECT_EFFVER], [']GL_PROJECT_MAJVER[.]GL_PROJECT_MINVER['])

AC_SUBST([PUBLISHER_UTF8ESC_NAME],
	['n4_escape_non_ascii(GL_PUBLISHER_NAME)'])

AC_OUTPUT

NC_SHADOW_MAYBE_OUTPUT

dnl  Create a file named `common-targets.txt` to be printed by `make help`
cat << 'END_HEREDOC' > 'make-targets.txt'
GNU make utility to maintain AC_PACKAGE_NAME

Commonly used targets:

NA_HELP_STRINGS(
	[[make], [make all]],
		[compile the library],
	[[make help]],
		[print this help page],
	[[make install], [make install-strip]],
		[install the library - if `make (all)` has not been launched yet, it
		will be launched first],
	[[make uninstall]],
		[delete all the files created by `make install`],
	[[make mostlyclean], [make clean], [make distclean]],
		[clean the source directory according to different degrees of fury],
	[[make clobber]],
		[launch `make uninstall` followed by `make clean`],
	[[make binary-release], [make manifest]],
		[create respectively a package and a manifest in the current location
		containing all the files that would be installed by `make
		install-strip` - if `make (all)` has not been launched yet, it will be
		launched first],
	[[make portable-builds]],
		[create a package containing all the files that would be installed by
		`make install-exec-strip`, but without any directory structure - if
		`make (all)` has not been launched yet, it will be launched first])

Package maintenance targets:

NA_HELP_STRINGS(
	[[make synch]],
		[merge all the files created by the `configure` option
		`--enable-extended-config=sandbox`],
	[[make dist(*)], [make source-release], [make authors-copy]],
		[create a re-distributable copy of this package, ignoring any local
		configuration; the `make dist(*)` targets are GNU Automake's built-in
		recipes for redistributing the source code of a generic package, while
		the last two targets have been written with this package in mind (but
		they all do their job well); in particular, `make source-release` is
		similar to `make dist`, with the difference that it tries to reproduce
		the shape of the package as close as possible to the original release;
		same goes for `make authors-copy`, although this goes a step further
		and re-creates the original package as after launching `make
		author-clean`],
	[[make maintainer-clean], [make author-clean], [make oblivion-clean]],
		[clean the source directory according to different degrees of fury
		beyond `make distclean` - unless you are a package maintainer you
		should not use these targets, as your machine might not have the tools
		for re-creating the files deleted by `make maintainer-clean`, `make
		author-clean` and `make oblivion-clean`],
	[[make distcheck]],
		[create a redistributable package and check that everything in it
		works as it is supposed to work],
	[[make git-clone]],
		[download the latest revision of ]AC_PACKAGE_NAME[ from the Git server
		(the `git` utility is required); note that this might not be an
		official release yet]
)

Optional variables:

NA_HELP_STRINGS(
	[[CC], [CFLAGS], [LDFLAGS], [etc.]],
		[Supported by `make (all)`, these variables allow to change on the fly
		the setup used to build single targets or the entire package, even when
		they have already been specified with different values to the
		`configure` script; type `./configure --help` for more information
		about these variables],
	[[DESTDIR]],
		[Supported by the `make install(-*)` and `make uninstall(-*)` targets,
		the `DESTDIR` variable allows to change the final installation path,
		independently of how the package has been configured; this variable can
		contain absolute paths only and defaults to `/` when omitted],
	[[ZIPTYPE]],
		[Supported by `make authors-copy`, `make binary-release`, `make
		git-clone`, `make portable-builds` and `make source-release`, this
		variable allows to change the compression format of the archives
		created; possible values are: `tar.gz`, `tar.xz`, `tgz`, `txz`, `zip`],
	[[DEST]],
		[Supported by `make authors-copy`, `make binary-release`, `make
		git-clone`, `make manifest`, `make portable-builds` and `make
		source-release`, the `DEST` variable allows to change the destination
		and possibly the compression format of the archives created (in the
		last case with the exception of `make manifest`, as this never produces
		archives); for example: `make DEST=']AC_PACKAGE_TARNAME[-edited.zip'
		portable-builds` creates a zip archive named
		`]AC_PACKAGE_TARNAME[-edited` instead of the default `.tar.xz` archive;
		if no recognizable extension is present, a directory will be created
		instead; archive formats supported are: `tar.gz`, `tar.xz`, `tgz`,
		`txz`, `zip`; this variable always overrides `ZIPTYPE`]
)

For further options, please refer to the documentation of GNU Automake.

END_HEREDOC

AC_MSG_NOTICE([all done, use `make help` to list the commonly used targets.])


dnl  **************************************************************************
dnl     NOTE:  About the macros whose names begin with `GL_`, `NA_`, `NC_`,
dnl            `NM_`, `NS_` `n4_`, and what these prefixes are, see
dnl            `autostuff/m4/not-autotools.m4`.
dnl  **************************************************************************


dnl  EOF

