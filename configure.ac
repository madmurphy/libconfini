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


dnl  ***************************************************************************
dnl     G E N E R A L   P U R P O S E   M A C R O S
dnl  ***************************************************************************


dnl  NA_SANITIZE_VARNAME(string)
dnl  ---------------------------
dnl
dnl  Replaces `/\W/g,` with `'_'` and `/^\d/` with `V$0`
dnl
dnl  Useful for sanitizing strings that need to be used as variable names in
dnl  several programming languages (Bash, C, JavaScript, etc.).
dnl
AC_DEFUN([NA_SANITIZE_VARNAME],
	[m4_if(m4_bregexp(m4_normalize([$1]), [[0-9]]), [0], [V])[]m4_translit(m4_normalize([$1]),
		[ !"#$%&\'()*+,-./:;<=>?@[\\]^`{|}~],
		[__________________________________])])

dnl  NA_SET_GLOBALLY(name1, [value1][, name2, [value2][, ... nameN, [valueN]]])
dnl  __________________________________________________________________________
dnl
dnl  For each `nameN`-`valueN` pair, creates a new argumentless macro named
dnl  `[GL_]nameN` (where the `GL_` prefix stands for "Global Literal") and a new
dnl  output substitution named `nameN`, both expanding to `valueN` when invoked
dnl
dnl  Requires: `NA_SANITIZE_VARNAME()`
dnl
AC_DEFUN([NA_SET_GLOBALLY], [
	m4_define([GL_]NA_SANITIZE_VARNAME([$1]), m4_normalize([$2]))
	AC_SUBST(NA_SANITIZE_VARNAME([$1]), ['m4_bpatsubst(m4_normalize([$2]), ['], ['\\''])'])
	m4_if(m4_eval([$# > 2]), [1], [NA_SET_GLOBALLY(m4_shift2($@))])
])

dnl  NA_REQ_PROGS(prog1, [descr1][, prog2, [descr2][, ... progN, [descrN]]])
dnl  _______________________________________________________________________
dnl
dnl  Checks whether one or more programs have been provided by the user or can
dnl  be retrieved automatically. For each program `progx` an uppercase bash
dnl  variable named `PROGX` containing the path where `progx` is located will be
dnl  created. If a program is not reachable and the user has not provided any
dnl  path for it an error will be generated. The program names given to this
dnl  function will be advertised among the `influential environment variables`
dnl  visible when launching `./configure --help`.
dnl
dnl  Requires: `NA_SANITIZE_VARNAME()`
dnl
AC_DEFUN([NA_REQ_PROGS], [
	AC_ARG_VAR(m4_toupper(NA_SANITIZE_VARNAME([$1])), m4_default_quoted(m4_normalize([$2]), [$1 utility]))
	AS_IF([test "x@S|@{]m4_toupper(NA_SANITIZE_VARNAME([$1]))[}" = x], [
		AC_PATH_PROG(m4_toupper(NA_SANITIZE_VARNAME([$1])), [$1])
		AS_IF([test "x@S|@{]m4_toupper(NA_SANITIZE_VARNAME([$1]))[}" = x], [AC_MSG_ERROR([$1 utility not found])])
	])
	m4_if(m4_eval([$# > 2]), [1], [NA_REQ_PROGS(m4_shift2($@))])
])

dnl  NM_SET_VERSION_ENVIRONMENT(pkgname, majver, minver, revver)
dnl  ___________________________________________________________
dnl
dnl  Gets the version environment for this package and sets the versioning
dnl  macros accordingly. This macro can be invoked only once. After being
dnl  invoked the following argumentless macros will be created:
dnl
dnl  - `NA_IS_MULTIVERSION`: expands to `1` if the `$MULTIVERSION_PACKAGE`
dnl    environment variable was set to any value other than `no` during the
dnl    the automake process, expands to `0` otherwise
dnl  - `NA_PROJECT_NAME`: expands to the argument `pkgname` passed to this macro
dnl  - `NA_PROJECT_MAJVER`: expands to the argument `majver` passed to this
dnl    macro
dnl  - `NA_PROJECT_MINVER`: expands to the argument `minver` passed to this
dnl    macro
dnl  - `NA_PROJECT_REVVER`: expands to the argument `revver` passed to this
dnl    macro
dnl  - `NA_PROJECT_DISTNAME` expands to `pkgname[]majver` when
dnl    `NA_IS_MULTIVERSION` expands to [yes], it expands to `pkgname` otherwise
dnl
AC_DEFUN_ONCE([NM_SET_VERSION_ENVIRONMENT], [
	m4_define([NA_PROJECT_NAME], [$1])
	m4_define([NA_PROJECT_MAJVER], [$2])
	m4_define([NA_PROJECT_MINVER], [$3])
	m4_define([NA_PROJECT_REVVER], [$4])
	m4_define([NA_IS_MULTIVERSION], m4_esyscmd_s([test "x${MULTIVERSION_PACKAGE}" = x -o "x${MULTIVERSION_PACKAGE}" = xno && echo '0' || echo '1']))
	m4_define([NA_PROJECT_DISTNAME], m4_if(NA_IS_MULTIVERSION, [1], NA_PROJECT_NAME[]NA_PROJECT_MAJVER, NA_PROJECT_NAME))
])


dnl  ***************************************************************************
dnl     P A C K A G E   M E T A D A T A
dnl  ***************************************************************************


AC_PREREQ([2.69])

NM_SET_VERSION_ENVIRONMENT([confini], [1], [10], [1])

AC_INIT([lib]NA_PROJECT_DISTNAME, NA_PROJECT_MAJVER[.]NA_PROJECT_MINVER[.]NA_PROJECT_REVVER)

NA_SET_GLOBALLY(
	[LIBRARY_DESCRIPTION],		[Yet another INI parser],
	[INTERFACE_NUMBER],			[1],
	[IMPLEMENTATION_NUMBER],	[1],
	[INTERFACE_COMPATIBILITY],	[1],
	[PACKAGE_REALNAME],			[lib]NA_PROJECT_NAME,
	[EFFECTIVE_VERSION],		NA_PROJECT_MAJVER[.]NA_PROJECT_MINVER
)

AC_SUBST([LIBRARY_DISTNAME], [NA_PROJECT_DISTNAME])

AC_SUBST([LIBRARY_NAME], [NA_PROJECT_NAME])

AC_SUBST([LIBRARY_MAJVER], [NA_PROJECT_MAJVER])


dnl  ***************************************************************************
dnl     A U T O C O N F   E N V I R O N M E N T
dnl  ***************************************************************************

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_FILES([
	Makefile
	src/]AC_PACKAGE_NAME[.pc:src/]GL_PACKAGE_REALNAME[.pc.in
	src/Makefile
])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC_C99

AC_PROG_LN_S

AC_CHECK_HEADERS([stdint.h stdlib.h])

AC_C_INLINE

AC_TYPE_INT8_T

AC_TYPE_SIZE_T

AC_TYPE_UINT16_T

AC_TYPE_UINT32_T

AC_TYPE_UINT8_T

AC_FUNC_MALLOC

AS_IF([test "x${pkgconfigdir}" = x], [pkgconfigdir='$(libdir)/pkgconfig'])

AC_ARG_WITH([pkgconfigdir],
	[AS_HELP_STRING([--with-pkgconfigdir=DIR],
		[pkgconfig files @<:@default=<lib dir>/pkgconfig@:>@])],
	[AS_IF([test "x${withval}" = x -o "x${with_pkgconfigdir}" = xyes],
		[AC_MSG_ERROR([a DIR value must be specified for option --with-pkgconfigdir=DIR])],
		[AS_IF([test "x${with_pkgconfigdir}" = xno],
			[AC_MSG_WARN([unrecognized option: --without-pkgconfigdir])],
			[pkgconfigdir="${withval}"])])],
	[:])

AC_SUBST([pkgconfigdir])

AC_ARG_VAR([RC], [Microsoft Windows resource compiler command])

AC_ARG_VAR([RCFLAGS], [Microsoft Windows resource compiler flags])

AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--disable-doc],
		[don't generate any documentation @<:@default=no@:>@])],
	[:],
	[enable_doc=yes])

AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples],
		[don't generate examples @<:@default=<disable-doc>@:>@])],
	[:],
	[enable_examples=check])

m4_if(NA_IS_MULTIVERSION, [1], [
	AC_PROG_SED
	NA_REQ_PROGS(
		[find],		[Unix find utility],
		[xargs],	[Unix xargs utility]
	)
])

AM_CONDITIONAL([MAJOR_VERSIONS_COEXIST], m4_if(NA_IS_MULTIVERSION, [1], [true], [false]))

AM_CONDITIONAL([ENABLE_DOC], [test "x${enable_doc}" != xno])

AS_IF([test "x${enable_examples}" = xcheck],
	[AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${enable_doc}" != xno])],
	[AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${enable_examples}" != xno])])


dnl  ***************************************************************************
dnl     C H E C K S   F O R   W I N D O W S
dnl  ***************************************************************************


AC_CANONICAL_HOST

AS_CASE([$host_os],
	[*mingw*], [
		na_platform_mswin=yes
		na_native_mswin=yes
	],
	[pw32*|*cygwin*], [
		na_platform_mswin=yes
		na_native_mswin=no
	],
	[*], [
		na_platform_mswin=no
		na_native_mswin=no
	])

AM_CONDITIONAL([PLATFORM_MSWIN], [test "x${na_platform_mswin}" = xyes])

AM_CONDITIONAL([NATIVE_MSWIN], [test "x${na_native_mswin}" = xyes])

LT_INIT([win32-dll])

AM_COND_IF([PLATFORM_MSWIN], [LT_PROG_RC])

AM_CONDITIONAL([HAVE_RC], [test "x${RC}" != x])


dnl  ***************************************************************************
dnl     F I N E   T U N I N G
dnl  ***************************************************************************


dnl  Optimization `-O3` seems to be working better than `-O2` when compiling
dnl  under native Microsoft Windows. In all other cases use `-O2`.
AM_COND_IF([NATIVE_MSWIN],
	[AC_SUBST([CODE_OPTIMIZATION], [3])],
	[AC_SUBST([CODE_OPTIMIZATION], [2])])


dnl  ***************************************************************************
dnl     O U T P U T
dnl  ***************************************************************************


AC_OUTPUT


dnl  ***************************************************************************
dnl     Note:  The `NA_` prefix (which stands for "Not Autotools"), the `NM_`
dnl            prefix (which stands for "Not autoMake") and the `GL_` prefix
dnl            (which stands for "Global Literal") are used with the purpose
dnl            of avoiding collisions with the default Autotools prefixes
dnl            `AC_`, `AM_`, `AS_`, `AX_`, `LT_`.
dnl  ***************************************************************************


dnl  EOF

