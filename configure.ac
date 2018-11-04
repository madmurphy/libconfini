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
dnl  General purpose macros
dnl  ***************************************************************************


dnl  NA_SET_GLOBAL_VAR(NAME, VALUE)
dnl
dnl  Creates a new argumentless macro named NA_`NAME` and a new output
dnl  substitution named `NAME`, both expanding to `VALUE` when invoked.
AC_DEFUN([NA_SET_GLOBAL_VAR], [
	m4_define([NA_$1], [$2])
	AC_SUBST([$1], ["$2"])
])


dnl  ***************************************************************************
dnl  Autoconf header
dnl  ***************************************************************************


AC_DEFUN([NA_LIBRARY_NAME], [confini])

AC_INIT([lib]NA_LIBRARY_NAME, [1.8])

NA_SET_GLOBAL_VAR([LIBRARY_DESCRIPTION], [Yet another INI parser])

NA_SET_GLOBAL_VAR([INTERFACE_NUMBER], [0])

NA_SET_GLOBAL_VAR([IMPLEMENTATION_NUMBER], [1])

NA_SET_GLOBAL_VAR([INTERFACE_COMPATIBILITY], [0])


dnl  ***************************************************************************
dnl  Autoconf environment
dnl  ***************************************************************************


AC_SUBST([LIBRARY_NAME], [NA_LIBRARY_NAME])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([
	Makefile
	src/]AC_PACKAGE_NAME[-]AC_PACKAGE_VERSION[.pc
	src/Makefile
])

AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--disable-doc],
		[Create package without documentation @<:@default=no@:>@]
	)],
	[:],
	[na_enable_doc=yes]
)

AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples],
		[Create package without examples @<:@default=no@:>@]
	)],
	[:],
	[na_enable_examples=yes]
)

AM_CONDITIONAL([ENABLE_DOC], [test "x${na_enable_doc}" != xno])

AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${na_enable_examples}" != xno])


dnl  ***************************************************************************
dnl  Check for Windows
dnl  ***************************************************************************


AC_CANONICAL_HOST


case ${host_os} in
	*mingw*)
		na_platform_mswin=yes
		na_native_mswin=yes
		;;
	pw32*|*cygwin*)
		na_platform_mswin=yes
		na_native_mswin=no
		;;
	*)
		na_platform_mswin=no
		na_native_mswin=no
		;;
esac


AM_CONDITIONAL([PLATFORM_MSWIN], [test "x${na_platform_mswin}" = xyes])

AM_CONDITIONAL([NATIVE_MSWIN], [test "x${na_native_mswin}" = xyes])

LT_INIT([win32-dll])

AM_COND_IF([PLATFORM_MSWIN], [LT_PROG_RC])

AM_CONDITIONAL([HAVE_RC], [test "x${RC}" != x])


dnl  ***************************************************************************
dnl  Output
dnl  ***************************************************************************


AC_OUTPUT


dnl  ***************************************************************************
dnl  Note:  The `NA_` prefix (which stands for "Not Autotools") is used
dnl         with the purpose of avoiding collisions with the default Autotools
dnl `       prefixes `AC_`, AM_`, `AS_`, `AX_`, `LT_`.
dnl  ***************************************************************************

dnl  EOF

