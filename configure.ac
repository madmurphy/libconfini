dnl  -*- Mode: M4; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-
dnl  Process this file with autoconf to produce a configure script.

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

dnl  This package has not been tested with previous versions of Autoconf...
AC_PREREQ([2.68])


dnl  **************************************************************************
dnl     P A C K A G E   M E T A D A T A
dnl  **************************************************************************

NM_SET_VERSION_ENVIRONMENT([confini], [1], [10], [2])

AC_INIT([lib]NM_PROJECT_NAME,
	NM_PROJECT_MAJVER[.]NM_PROJECT_MINVER[.]NM_PROJECT_REVVER,
	[madmurphy333@gmail.com],
	[lib]NM_PROJECT_DISTNAME,
	[https://madmurphy.github.io/libconfini])

NC_SET_GLOBALLY(
	[INTERFACE_NUMBER],			[1],
	[IMPLEMENTATION_NUMBER],	[2],
	[INTERFACE_COMPATIBILITY],	[1],
	[PROJECT_DESCRIPTION],		[Yet another INI parser],
	[LICENSE_LONGNAME],			[GNU General Public License, version 3 or any
								later version],
	[LICENSE_SHORTNAME],		[GPL3+],
	[AUTHOR_NAME],				[Stefano Gioffré],
	[AUTHOR_NICKNAME],			[madmurphy]
)


dnl  **************************************************************************
dnl     R E N A M I N G   R U L E S
dnl  **************************************************************************

AC_SUBST([PROJECT_NAME], [NM_PROJECT_NAME])

AS_VAR_SET([na_tmppref],
	["$(test "x${program_prefix}" = xNONE || echo "${program_prefix}" \
		| sed 's/@<:@^@<:@:alnum:@:>@@:>@\+/-/g;s/@<:@A-Z@:>@/\L&/g')"])

AS_VAR_SET([na_tmpsuff],
	["$(test "x${program_suffix}" = xNONE || echo "${program_suffix}" \
		| sed 's/@<:@^@<:@:alnum:@:>@@:>@\+/-/g;s/@<:@A-Z@:>@/\L&/g')"])

AS_VAR_SET([na_tmptransf], ["${program_transform_name}"])

AS_VAR_SET([na_tmpprobe],
	["$(echo "${na_tmppref}]NM_PROJECT_NAME[${na_tmpsuff}" | \
		sed "${na_tmptransf}")"])

AM_CONDITIONAL([RENAME_PACKAGE],
	[test "x${na_tmpprobe}" != 'x]NM_PROJECT_NAME['])

AM_COND_IF([RENAME_PACKAGE], [
	AC_SUBST([PROJECT_CONFNAME], ["${na_tmpprobe}"])
	AC_SUBST([PACKAGE_CONFNAME], ["lib${PROJECT_CONFNAME}"])
	AC_SUBST([PROJECT_LOCALNAME], ["${PROJECT_CONFNAME}"])
	AC_SUBST([PACKAGE_LOCALNAME], ["${PACKAGE_CONFNAME}"])
	AC_MSG_NOTICE([renaming ]AC_PACKAGE_NAME[ to ${PACKAGE_CONFNAME}...])
	AS_VAR_SET([docdir], ['$(datarootdir)/doc/$(PACKAGE_LOCALNAME)'])
], [
	AC_SUBST([PROJECT_CONFNAME], NM_PROJECT_NAME)
	AC_SUBST([PACKAGE_CONFNAME], AC_PACKAGE_NAME)
	AC_SUBST([PROJECT_LOCALNAME], NM_PROJECT_DISTNAME)
	AC_SUBST([PACKAGE_LOCALNAME], AC_PACKAGE_TARNAME)
])

dnl  Always true when `RENAME_PACKAGE` is `true`
AM_CONDITIONAL([THIS_IS_THE_ONLY_VERSION],
	NM_IF_MULTIVERSION([test "x${PROJECT_CONFNAME}" != 'x]NM_PROJECT_NAME['],
		[true]))

dnl  Prevent any further transformations from Autoconf (unless
dnl  `AC_CANONICAL_TARGET` is invoked after these lines)
AS_VAR_SET([program_prefix], ['NONE'])
AS_VAR_SET([program_suffix], ['NONE'])
AS_VAR_SET([program_transform_name], ['s,x,x,'])

NS_UNSET([na_tmpprobe], [na_tmptransf], [na_tmpsuff], [na_tmppref])


dnl  **************************************************************************
dnl     A U T O C O N F   E N V I R O N M E N T
dnl  **************************************************************************

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_AUX_DIR([build-aux])

NC_CONFIG_SHADOW_DIR([autostuff/shadows])

AC_CONFIG_FILES([
	Makefile
	src/${PACKAGE_LOCALNAME}.pc:src/]AC_PACKAGE_NAME[.pc.in
	src/Makefile
])

NC_THREATEN_FILES([package.json], [src/winres.rc])

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

AC_ARG_VAR([RC], [Microsoft Windows resource compiler command])

AC_ARG_VAR([RCFLAGS], [Microsoft Windows resource compiler flags])

NC_REQ_PROGS(
	[find],		[Unix find utility],
	[xargs],	[Unix xargs utility]
)

AC_ARG_ENABLE([version-info],
	[  --enable-version-info,m4_newline()AS_HELP_STRING([--disable-version-info],
		[enable/disable libtool versioning system @<:@default=SYSTEM-DEPENDENT@:>@])],
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

dnl  PREPARE_MANPAGES(page1, renamed-page1[, ... pageN, renamed-pageN])
dnl  ------------------------------------------------------------------
dnl
dnl  Applies renamings (if any) to man pages and sets the `notrans_man_MANS`
dnl  target
dnl
AC_DEFUN_ONCE([PREPARE_MANPAGES], [
	AM_COND_IF([INCLUDE_DOC], [
		test -e 'no-dist/man3' && rm -r 'no-dist/man3'
		AS_MKDIR_P([no-dist/man3])
		AM_COND_IF([RENAME_PACKAGE], [
			m4_for([_idx_], [1], [$#], [2], [
				cp "docs/man/man3/m4_normalize(m4_argn(_idx_, $@))" \
					"no-dist/man3/m4_normalize(m4_argn(m4_incr(_idx_), $@))";
			])
		], [
			m4_for([_idx_], [1], [$#], [2], [
				cp "docs/man/man3/m4_normalize(m4_argn(_idx_, $@))" \
					'no-dist/man3/';
			])
			NM_IF_MULTIVERSION([
				mv 'no-dist/man3/AC_PACKAGE_NAME.3' \
					'no-dist/man3/AC_PACKAGE_TARNAME.3'
				mv 'no-dist/man3/NM_PROJECT_NAME.h.3' \
					'no-dist/man3/NM_PROJECT_NAME-NM_PROJECT_MAJVER.h.3'
			])
		])
		dnl  See `Makefile.am` for this workaround
		AC_SUBST([POPULATE_MANPAGE_LIST], 
			["notrans_man_MANS = $(echo no-dist/man3/*)"])
	], [
		AC_SUBST([POPULATE_MANPAGE_LIST], ["notrans_man_MANS ="])
	])
])

dnl  Man pages go here, each page followed by a safe cohabitation renaming
PREPARE_MANPAGES(
	AC_PACKAGE_NAME[.3],	[${PACKAGE_LOCALNAME}.3],
	NM_PROJECT_NAME[.h.3],	[${PROJECT_CONFNAME}.h.3],
	[IniStatistics.3],		[${PACKAGE_LOCALNAME}-IniStatistics.3],
	[IniFormat.3],			[${PACKAGE_LOCALNAME}-IniFormat.3],
	[IniDispatch.3],		[${PACKAGE_LOCALNAME}-IniDispatch.3]
)

AC_SUBST([PROJECT_MAJVER], [NM_PROJECT_MAJVER])

AC_SUBST([PROJECT_MINVER], [NM_PROJECT_MINVER])

AC_SUBST([PROJECT_REVVER], [NM_PROJECT_REVVER])

AC_SUBST([PROJECT_EFFVER], [NM_PROJECT_MAJVER.NM_PROJECT_MINVER])

AC_SUBST([AUTHOR_UTF8ESC_NAME],
	['n4_escape_non_ascii(GL_AUTHOR_NAME)'])

# Other extended config and renaming rules...
AM_COND_IF([RENAME_PACKAGE], [
	AM_COND_IF([HAVE_EXTENDED_CONFIG], [
		NC_MSG_WARNBOX([Extended configuration has been called on a renamed
			package. Changes will be reverted with `make *clean` and will not
			be re-distributed with `make dist`!])
		AC_SUBST([renaming_cleanhook],
			[']m4_foreach([_CLEAN_ITER_], [NC_THREATENED_LIST], [[ \
				test -f ]_CLEAN_ITER_.dist[ && \
					mv ]_CLEAN_ITER_[.dist ]_CLEAN_ITER_[; ]])['])
		AC_SUBST([renaming_disthook],
			[']m4_foreach([_DIST_ITER_], [NC_THREATENED_LIST], [[ \
				test -f ]_DIST_ITER_.dist[ && \
					cp ]_DIST_ITER_[.dist ]$(distdir)/_DIST_ITER_[; ]])['])
	])
	AM_COND_IF([HAVE_UPDATES], [
		AC_SUBST([synch_actions],
			[']m4_foreach([_BACK_ITER_], [NC_THREATENED_LIST], [[ \
				test -f ]_BACK_ITER_.dist[ || \
					cp ]_BACK_ITER_[ ]_BACK_ITER_[.dist; \
					cp ]NC_CONFNEW_DIR[/]_BACK_ITER_[ ]_BACK_ITER_[; ]])['])
	], [
		m4_foreach([_BACK_ITER_], [NC_THREATENED_LIST],
			[test -f ]_BACK_ITER_.dist[ || cp ]_BACK_ITER_[ ]_BACK_ITER_[.dist;])
		m4_pattern_allow([^AM_V_GEN$])
		AC_SUBST([synch_actions],
			['$(AM_V_GEN)echo '\''Launch `configure --enable-extended-config=sandbox` to use `make synch`.'\'])
	])
], [
	AM_COND_IF([HAVE_UPDATES],
		[AC_SUBST([synch_actions],
			[']m4_foreach([_BACK_ITER_], [NC_THREATENED_LIST],
				[[cp ]NC_CONFNEW_DIR[/]_BACK_ITER_[ ]_BACK_ITER_[; ]])['])],
		[AC_SUBST([synch_actions],
				['$(AM_V_GEN)echo '\''Launch `configure --enable-extended-config=sandbox` to use `make synch`.'\'])])
])

AC_OUTPUT

NC_SHADOW_MAYBE_OUTPUT


dnl  **************************************************************************
dnl     Note:  About the macros whose names begin with `GL_`, `NA_`, `NC_`,
dnl            `NM_`, `n4_`, and what these prefixes are, see
dnl            `autostuff/m4/not-autotools.m4`.
dnl  **************************************************************************


dnl  EOF

