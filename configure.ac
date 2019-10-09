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
		[10])[.]n4_mem([GL_PROJECT_REVVER], [3]),
	[madmurphy333@gmail.com],
	[lib]GL_PROJECT_NAME,
	[https://madmurphy.github.io/libconfini])

NC_SET_GLOBALLY(
	[INTERFACE_NUMBER],			[1],
	[IMPLEMENTATION_NUMBER],	[2],
	[INTERFACE_COMPATIBILITY],	[1],
	[PROJECT_DESCRIPTION],		[Yet another INI parser],
	[LICENSE_LONGNAME],			[GNU General Public License, version 3 or any
								later version],
	[LICENSE_SHORTNAME],		[GPL3+],
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

AS_VAR_SET([na_tmptransf], ["${program_transform_name}"])

AC_SUBST([PROJECT_CONFNAME],
	["$(echo "${na_tmppref}]GL_PROJECT_NAME[${na_tmpsuff}" | \
		sed "${na_tmptransf}")"])

AC_SUBST([PACKAGE_CONFNAME], ["lib${PROJECT_CONFNAME}"])

AM_CONDITIONAL([RENAME_PACKAGE],
	[test "x${PROJECT_CONFNAME}" != 'x]GL_PROJECT_NAME['])

AC_ARG_WITH([other_versions],
	[AS_HELP_STRING([--with-other-versions],
		[make this package able to cohabitate with other major versions of
		itself @<:@default=no@:>@])],
	[:],
	[AS_VAR_SET([with_other_versions], ['no'])])

AM_CONDITIONAL([MULTIVERSION],
	[test "x${with_other_versions}" != xno])

AM_CONDITIONAL([HAVE_TRANSFORMATIONS],
	[test "x${with_other_versions}" != xno -o \
		"x${PROJECT_CONFNAME}" != 'x]GL_PROJECT_NAME['])

AM_COND_IF([MULTIVERSION], [
	AS_VAR_SET([na_tmpvsuff1], [']GL_PROJECT_MAJVER['])
	AS_VAR_SET([na_tmpvsuff2], ['-]GL_PROJECT_MAJVER['])
])

AM_COND_IF([RENAME_PACKAGE],
	[AC_MSG_NOTICE([renaming ]AC_PACKAGE_NAME[ to ${PACKAGE_CONFNAME}...])])

AC_SUBST([PROJECT_LOCALNAME], ["${PROJECT_CONFNAME}${na_tmpvsuff1}"])

AC_SUBST([PACKAGE_LOCALNAME], ["${PACKAGE_CONFNAME}${na_tmpvsuff1}"])

AC_SUBST([LIBRARY_HEADERNAME], ["${PROJECT_CONFNAME}${na_tmpvsuff2}"])

AS_VAR_SET([docdir], ['$(datarootdir)/doc/$(PACKAGE_LOCALNAME)'])

NS_UNSET([na_tmptransf], [na_tmpsuff], [na_tmppref], [na_tmpvsuff1],
	[na_tmpvsuff2])

dnl  Prevent any further transformations from GNU Autoconf (unless
dnl  `AC_CANONICAL_TARGET` is invoked after these lines)
AS_VAR_SET([program_prefix], ['NONE'])
AS_VAR_SET([program_suffix], ['NONE'])
AS_VAR_SET([program_transform_name], ['s,x,x,'])


dnl  **************************************************************************
dnl     A U T O C O N F   E N V I R O N M E N T
dnl  **************************************************************************

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_AUX_DIR([build-aux])

NC_CONFIG_SHADOW_DIR([autostuff/shadows])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_ARG_PROGRAM

AC_PROG_CC_C99

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

AC_ARG_VAR([RC], [Microsoft Windows resource compiler command])

AC_ARG_VAR([RCFLAGS], [Microsoft Windows resource compiler flags])

NC_REQ_PROGS(
	[find],		[Unix find utility],
	[xargs],	[Unix xargs utility]
)

AS_IF([test "x${pkgconfigdir}" = x],
	[AS_VAR_SET([pkgconfigdir], ['$(libdir)/pkgconfig'])])

AC_ARG_WITH([pkgconfigdir],
	[AS_HELP_STRING([--with-pkgconfigdir=DIR],
		[pkgconfig files @<:@default=<lib dir>/pkgconfig@:>@])],
	[AS_IF([test "x${withval}" = x -o "x${with_pkgconfigdir}" = xyes],
		[AC_MSG_ERROR([a DIR value must be specified for option --with-pkgconfigdir=DIR])],
		[AS_IF([test "x${with_pkgconfigdir}" = xno],
			[AC_MSG_WARN([unrecognized option: --without-pkgconfigdir])],
			[AS_VAR_SET([pkgconfigdir], ["${withval}"])])])],
	[:])

AC_SUBST([pkgconfigdir])

AC_ARG_ENABLE([version-info],
	[NA_HELP_STRINGS([[--enable-version-info], [--disable-version-info]],
		[enable/disable libtool versioning system
		@<:@default=SYSTEM-DEPENDENT@:>@])],
	[:],
	[AS_VAR_SET([enable_version_info], ['check'])])

AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--disable-doc],
		[don't generate any documentation @<:@default=no@:>@])],
	[:],
	[AS_VAR_SET([enable_doc], ['yes'])])

AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples],
		[don't generate examples @<:@default=<disable-doc>@:>@])],
	[:],
	[AS_VAR_SET([enable_examples], ['check'])])

AM_CONDITIONAL([INCLUDE_DOC], [test "x${enable_doc}" != xno])

AS_IF([test "x${enable_examples}" = xcheck],
	[AM_CONDITIONAL([INCLUDE_EXAMPLES], [test "x${enable_doc}" != xno])],
	[AM_CONDITIONAL([INCLUDE_EXAMPLES], [test "x${enable_examples}" != xno])])


dnl  **************************************************************************
dnl     P L A T F O R M   C H E C K S
dnl  **************************************************************************

AC_CANONICAL_HOST

AS_CASE([$host_os],
	[*mingw*], [
		AS_VAR_SET([na_platform_mswin], [yes])
		AS_VAR_SET([na_native_mswin], [yes])
		AM_CONDITIONAL([SKIP_VERSIONING],
			[test "x${enable_version_info}" = xno -o \
				"x${enable_version_info}" = xcheck])
	],
	[pw32*|*cygwin*|*msys*], [
		AS_VAR_SET([na_platform_mswin], [yes])
		AS_VAR_SET([na_native_mswin], [no])
		AM_CONDITIONAL([SKIP_VERSIONING],
			[test "x${enable_version_info}" = xno])
	],
	[*], [
		AS_VAR_SET([na_platform_mswin], [no])
		AS_VAR_SET([na_native_mswin], [no])
		AM_CONDITIONAL([SKIP_VERSIONING],
			[test "x${enable_version_info}" = xno])
	])

AM_CONDITIONAL([PLATFORM_MSWIN], [test "x${na_platform_mswin}" = xyes])

AM_CONDITIONAL([NATIVE_MSWIN], [test "x${na_native_mswin}" = xyes])

NS_UNSET([na_platform_mswin], [na_native_mswin])

LT_INIT([win32-dll])

AM_COND_IF([PLATFORM_MSWIN], [LT_PROG_RC])

AM_CONDITIONAL([HAVE_RC], [test "x${RC}" != x])


dnl  **************************************************************************
dnl     F I N E   T U N I N G
dnl  **************************************************************************

dnl  Optimization `-O3` seems to be working better than `-O2` when compiling
dnl  under native Microsoft Windows. In all other cases use `-O2`.
AM_COND_IF([NATIVE_MSWIN],
	[AC_SUBST([CODE_OPTIMIZATION], [3])],
	[AC_SUBST([CODE_OPTIMIZATION], [2])])


dnl  **************************************************************************
dnl     O U T P U T
dnl  **************************************************************************

AC_CONFIG_FILES([
	Makefile
	src/${PACKAGE_LOCALNAME}.pc:src/]AC_PACKAGE_NAME[.pc.in
	src/Makefile
])

NC_THREATEN_FILES([package.json], [src/winres.rc])

dnl  PREPARE_MANPAGES(page1, renamed-page1[, ... pageN, renamed-pageN])
dnl  ------------------------------------------------------------------
dnl
dnl  Apply renamings (if any) to man pages and set the `notrans_man_MANS`
dnl  variable
dnl
AC_DEFUN_ONCE([PREPARE_MANPAGES], [
	AM_COND_IF([INCLUDE_DOC], [
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
		AC_SUBST([POPULATE_MANPAGE_LIST], 
			["notrans_man_MANS = $(echo no-dist/man3/*)"])
	], [
		AC_SUBST([POPULATE_MANPAGE_LIST], ["notrans_man_MANS ="])
	])
])

dnl  Man pages go here, each page accompanied by a safe cohabitation renaming
dnl  and expected to be found in `${srcdir}/docs/man/man3`
PREPARE_MANPAGES(
	AC_PACKAGE_NAME[.3],	[${PACKAGE_LOCALNAME}.3],
	GL_PROJECT_NAME[.h.3],	[${LIBRARY_HEADERNAME}.h.3],
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
		AC_SUBST([renaming_cleanhook],
			[']m4_foreach([_iter_], [NC_THREATENED_LIST], [[ \
				test -f '\''$(srcdir)/'"]_iter_["'.dist'\'' && \
					mv '\''$(srcdir)/'"]_iter_["'.dist'\'' \
					'\''$(srcdir)/'"]_iter_["\'' || echo; \
			]])['])
		AC_SUBST([renaming_disthook],
			[']m4_foreach([_iter_], [NC_THREATENED_LIST], [[ \
				test -f '\''$(srcdir)/'"]_iter_["'.dist'\'' && \
					cp '\''$(srcdir)/'"]_iter_["'.dist'\'' \
					'\''$(distdir)/'"]_iter_["\'' || echo; \
			]])['])
	])
	AM_COND_IF([HAVE_UPDATES], [
		AC_SUBST([synch_actions],
			[']m4_foreach([_iter_], [NC_THREATENED_LIST], [[ \
				test -f '\''$(srcdir)/'"]_iter_["'.dist'\'' || \
					cp '\''$(srcdir)/'"]_iter_["\'' \
					'\''$(srcdir)/'"]_iter_["'.dist'\''; \
				cp '\''$(CONFNEW_DIR)/'"]_iter_["\'' \
					'\''$(srcdir)/'"]_iter_["\'' && \
					echo '\''File $(srcdir)/'"]_iter_["' successfully \
					updated.'\'' || echo '\''File $(CONFNEW_DIR)/'"]_iter_["' \
					not found.'\''; \
			]])['])
	], [
		m4_foreach([_iter_], [NC_THREATENED_LIST],
			[test -f "${srcdir}/]_iter_.dist[" || \
				cp "${srcdir}/]_iter_[" "${srcdir}/]_iter_[.dist" || \
				echo "File ${srcdir}/]NC_CONFNEW_DIR[/]_iter_[ not found.";])
		AC_SUBST([synch_actions],
			['echo '\''Launch `$(srcdir)/configure @<:@...@:>@ \
					--enable-extended-config=sandbox` to use `make synch`.'\'])
	])
], [
	AM_COND_IF([HAVE_UPDATES],
		[AC_SUBST([synch_actions],
			[']m4_foreach([_iter_], [NC_THREATENED_LIST], [[ \
				test -f '\''$(CONFNEW_DIR)/'"]_iter_["\'' && \
					cp '\''$(CONFNEW_DIR)/'"]_iter_["\'' \
					'\''$(srcdir)/'"]_iter_["\'' && \
					echo '\''File $(srcdir)/'"]_iter_["' successfully \
					updated.'\'' || echo '\''File $(CONFNEW_DIR)/'"]_iter_["' \
					not found.'\''; \
			]])['])],
		[AC_SUBST([synch_actions],
				['echo '\''Launch `$(srcdir)/configure @<:@...@:>@ \
					--enable-extended-config=sandbox` to use `make synch`.'\'])
	])
])

AC_SUBST([PROJECT_MAJVER], [GL_PROJECT_MAJVER])

AC_SUBST([PROJECT_MINVER], [GL_PROJECT_MINVER])

AC_SUBST([PROJECT_REVVER], [GL_PROJECT_REVVER])

AC_SUBST([PROJECT_EFFVER], [GL_PROJECT_MAJVER.GL_PROJECT_MINVER])

AC_SUBST([PUBLISHER_UTF8ESC_NAME],
	['n4_escape_non_ascii(GL_PUBLISHER_NAME)'])

AC_OUTPUT

NC_SHADOW_MAYBE_OUTPUT

dnl  Create a file named `common-targets.txt` to be printed by `make help`
cat << 'END_HEREDOC' > 'make-targets.txt'
GNU make utility to maintain AC_PACKAGE_STRING

Commonly used targets:

NA_HELP_STRINGS(
	[[make], [make all]],
		[compile the library],
	[[make help]],
		[print this help page],
	[[make install]],
		[install the library - if `make (all)` has not been launched yet, it
		will be launched first],
	[[make uninstall]],
		[delete all the files created by `make install`],
	[[make mostlyclean], [make clean], [make distclean]],
		[clean the source directory according to different degrees of fury],
	[[make clobber]],
		[launch `make uninstall` followed by `make clean`],
	[[make binary-release], [make binary-image], [make install-manifest]],
		[create respectively a package, a subdirectory or a manifest in the
		current location containing all the files that would be installed by
		`make install` - if `make (all)` has not been launched yet, it will be
		launched first])

Package maintenance targets:

NA_HELP_STRINGS(
	[[make synch]],
		[merge all the files created by the `configure` argument
		`--enable-extended-config=sandbox`],
	[[make dist], [make dist-gzip|-bzip2|-lzip|-xz|-zip], [make distdir],
	[make source-release], [make source-image], [make authors-copy],
	[make authors-suitcase]],
		[create a re-distributable copy of this package, ignoring any local
		configuration; the `make dist*` targets are GNU Automake's built-in
		recipes for redistributing the source code of a generic package, while
		the other targets have been written with this package in mind (but they
		all do their job well); in particular, `make source-release` and `make
		source-image` are very similar to `make dist` and `make distdir`
		respectively, but the former try to reproduce the shape of the package
		as close as possible to the original release; same goes for `make
		authors-copy` and `make authors-suitcase`, although these go a step
		further and distribute the original package as after launching `make
		author-clean`],
	[[make maintainer-clean], [make author-clean]],
		[clean the source directory according to different degrees of fury
		beyond `make distclean` - unless you are a package maintainer you
		should not use these targets, as your machine might not have the tools
		for re-creating the files deleted by `make maintainer-clean` and `make
		author-clean`],
	[[make distcheck]],
		[create a redistributable package and check that everything in it
		works as it is supposed to work]
)

For further options, please refer to the documentation of GNU Automake.

END_HEREDOC

AC_MSG_NOTICE([all done, use `make help` to list the commonly used targets.])


dnl  **************************************************************************
dnl     Note:  About the macros whose names begin with `GL_`, `NA_`, `NC_`,
dnl            `NM_`, `n4_`, and what these prefixes are, see
dnl            `autostuff/m4/not-autotools.m4`.
dnl  **************************************************************************


dnl  EOF

