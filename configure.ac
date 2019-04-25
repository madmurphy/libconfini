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
dnl  G E N E R A L   P U R P O S E   M A C R O S
dnl  ***************************************************************************


dnl  ***************************************************************************
dnl  NA_UP_WORDS_ONLY(string)
dnl
dnl  Replaces `/[a-z]/g` with `/[A-Z]/g` and `/\W/g,` with `'_'`,
dnl  ***************************************************************************
m4_define([NA_UP_WORDS_ONLY],
	[m4_translit([$1],
		[ !"#$%&\'()*+,./0123456789:;<=>?@[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~],
		[________________________________________ABCDEFGHIJKLMNOPQRSTUVWXYZ____])])

dnl  ***************************************************************************
dnl  NA_SET_GLOBALLY(name, value)
dnl
dnl  Creates a new argumentless macro named `[NA_]name` and a new output
dnl  substitution named `name`, both expanding to `value` when invoked. This
dnl  macro can be invoked only after having invoked `AC_INIT()`
dnl  ***************************************************************************
AC_DEFUN([NA_SET_GLOBALLY], [
	m4_define([NA_$1], [$2])
	AC_SUBST([$1], ["$2"])
])

dnl  ***************************************************************************
dnl  NA_REQ_PROGS(prog1, descr1[, prog2, descr2[, ... progN, ... descrN]]])
dnl
dnl  Checks whether one or more programs have been provided by the user or can
dnl  be retrieved automatically. For each program `progx` an uppercase bash
dnl  variable named `PROGX` containing the path where `progx` is located will be
dnl  created. If a program is not reachable and the user has not provided any
dnl  path for it an error will be generated. The program names given to this
dnl  function will be advertised among the `influential environment variables`
dnl  visible when launching `./configure --help`.
dnl
dnl  Requires: `NA_UP_WORDS_ONLY()`
dnl  ***************************************************************************
AC_DEFUN([NA_REQ_PROGS], [
	m4_if([$#], [0], [], [
		AC_ARG_VAR(NA_UP_WORDS_ONLY([$1]), [$2])
		AS_IF([test "x@S|@{]NA_UP_WORDS_ONLY([$1])[}" = x], [
			AC_PATH_PROG(NA_UP_WORDS_ONLY([$1]), [$1])
			AS_IF([test "x@S|@{]NA_UP_WORDS_ONLY([$1])[}" = x], [AC_MSG_ERROR([$1 utility not found])])
		])
		m4_if(m4_eval([$# + 1 >> 1]), [1], [], [NA_REQ_PROGS(m4_shift2($*))])
	])
])

dnl  ***************************************************************************
dnl  NA_GET_VERSION_ENVIRONMENT(libname, majver, minver, revver)
dnl
dnl  Gets the version environment for this package and sets the versioning
dnl  macros accordingly. This macro can be invoked only once. After being
dnl  invoked the following argumentless macros will be created:
dnl
dnl  - `NA_MULTIVERSION_LIB`: expands to `1` if the `$MULTIVERSION_LIB`
dnl    environment variable was set to any value other than `no` during the
dnl    the automake process, expands to `0` otherwise
dnl  - `NA_LIBRARY_NAME`: expands to the argument `libname` passed to this macro
dnl  - `NA_LIBRARY_MAJVER`: expands to the argument `majver` passed to this
dnl    macro
dnl  - `NA_LIBRARY_MINVER`: expands to the argument `minver` passed to this
dnl    macro
dnl  - `NA_LIBRARY_REVVER`: expands to the argument `revver` passed to this
dnl    macro
dnl  - `NA_LIBRARY_LOCALNAME` expands to `libname[]majver` when
dnl    `NA_MULTIVERSION_LIB` expands to [yes], it expands to `libname` otherwise
dnl  ***************************************************************************
AC_DEFUN_ONCE([NA_GET_VERSION_ENVIRONMENT], [
	m4_define([NA_LIBRARY_NAME], [$1])
	m4_define([NA_LIBRARY_MAJVER], [$2])
	m4_define([NA_LIBRARY_MINVER], [$3])
	m4_define([NA_LIBRARY_REVVER], [$4])
	m4_define([NA_MULTIVERSION_LIB], m4_esyscmd_s([test "x${MULTIVERSION_LIB}" = x -o "x${MULTIVERSION_LIB}" = xno && echo '0' || echo '1']))
	m4_define([NA_LIBRARY_LOCALNAME], m4_if(NA_MULTIVERSION_LIB, [1], NA_LIBRARY_NAME[]NA_LIBRARY_MAJVER, NA_LIBRARY_NAME))
])


dnl  ***************************************************************************
dnl  P A C K A G E   M E T A D A T A
dnl  ***************************************************************************


NA_GET_VERSION_ENVIRONMENT([confini], [1], [9], [2])

AC_INIT([lib]NA_LIBRARY_LOCALNAME, NA_LIBRARY_MAJVER[.]NA_LIBRARY_MINVER[.]NA_LIBRARY_REVVER)

NA_SET_GLOBALLY([LIBRARY_DESCRIPTION], [Yet another INI parser])

NA_SET_GLOBALLY([INTERFACE_NUMBER], [0])

NA_SET_GLOBALLY([IMPLEMENTATION_NUMBER], [6])

NA_SET_GLOBALLY([INTERFACE_COMPATIBILITY], [0])


dnl  ***************************************************************************
dnl  A U T O C O N F   E N V I R O N M E N T
dnl  ***************************************************************************


NA_SET_GLOBALLY([PACKAGE_REALNAME], [lib]NA_LIBRARY_NAME)

NA_SET_GLOBALLY([EFFECTIVE_VERSION], NA_LIBRARY_MAJVER[.]NA_LIBRARY_MINVER)

AC_SUBST([LIBRARY_LOCALNAME], [NA_LIBRARY_LOCALNAME])

AC_SUBST([LIBRARY_NAME], [NA_LIBRARY_NAME])

AC_SUBST([LIBRARY_MAJVER], [NA_LIBRARY_MAJVER])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC_C99

AC_PROG_LN_S

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([
	Makefile
	src/]AC_PACKAGE_NAME[.pc:src/]NA_PACKAGE_REALNAME[.pc.in
	src/Makefile
])

pkgconfigdir='$(libdir)/pkgconfig'

AC_ARG_WITH([pkgconfigdir],
	[AS_HELP_STRING([--with-pkgconfigdir=DIR],
		[pkgconfig files @<:@default=<lib dir>/pkgconfig@:>@]
	)],
	[AS_IF([test "x${withval}" = x -o "x${with_pkgconfigdir}" = xyes],
		[AC_MSG_ERROR([a DIR value must be specified for option --with-pkgconfigdir=DIR])],
		[AS_IF([test "x${with_pkgconfigdir}" = xno],
			[AC_MSG_WARN([unrecognized option: --without-pkgconfigdir])],
			[pkgconfigdir="${withval}"]
		)]
	)],
	[:]
)

AC_ARG_VAR([RC], [Microsoft Windows resource compiler command])

AC_ARG_VAR([RCFLAGS], [Microsoft Windows resource compiler flags])

AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--disable-doc],
		[don't generate any documentation @<:@default=no@:>@]
	)],
	[:],
	[enable_doc=yes]
)

AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples],
		[don't generate examples @<:@default=<disable-doc>@:>@]
	)],
	[:],
	[enable_examples=check]
)

m4_if(NA_MULTIVERSION_LIB, [1], [NA_REQ_PROGS(
	[find],		[Unix find utility],
	[xargs],	[Unix xargs utility]
)])

AM_CONDITIONAL([MAJOR_VERSIONS_COEXIST], m4_if(NA_MULTIVERSION_LIB, [1], [true], [false]))

AM_CONDITIONAL([ENABLE_DOC], [test "x${enable_doc}" != xno])

AS_IF([test "x${enable_examples}" = xcheck],
	[AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${enable_doc}" != xno])],
	[AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${enable_examples}" != xno])]
)

AC_SUBST([pkgconfigdir])


dnl  ***************************************************************************
dnl  C H E C K S   F O R   W I N D O W S
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
	]
)

AM_CONDITIONAL([PLATFORM_MSWIN], [test "x${na_platform_mswin}" = xyes])

AM_CONDITIONAL([NATIVE_MSWIN], [test "x${na_native_mswin}" = xyes])

LT_INIT([win32-dll])

AM_COND_IF([PLATFORM_MSWIN], [LT_PROG_RC])

AM_CONDITIONAL([HAVE_RC], [test "x${RC}" != x])


dnl  ***************************************************************************
dnl  F I N E   T U N I N G
dnl  ***************************************************************************


dnl  Optimization `-O3` seems to be working better than `-O2` when compiling
dnl  under native Microsoft Windows. In all other cases use `-O2`.
AM_COND_IF([NATIVE_MSWIN],
	[AC_SUBST([CODE_OPTIMIZATION], [3])],
	[AC_SUBST([CODE_OPTIMIZATION], [2])]
)


dnl  ***************************************************************************
dnl  O U T P U T
dnl  ***************************************************************************


AC_OUTPUT


dnl  ***************************************************************************
dnl  Note:  The `NA_` prefix (which stands for "Not Autotools") is used
dnl         with the purpose of avoiding collisions with the default Autotools
dnl `       prefixes `AC_`, AM_`, `AS_`, `AX_`, `LT_`.
dnl  ***************************************************************************


dnl  EOF

