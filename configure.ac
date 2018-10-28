dnl Process this file with autoconf to produce a configure script.

m4_define([AC_LIBRARY_NAME], [confini])

m4_define([AC_LIBRARY_DESCRIPTION], ['Yet another INI parser'])

AC_INIT([lib]AC_LIBRARY_NAME, [1.8])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--disable-doc],
		[Create package without documentation @<:@default=no@:>@]
	)],
	[:],
	[sh_enable_doc=yes]
)

AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples],
		[Create package without examples @<:@default=no@:>@]
	)],
	[:],
	[sh_enable_examples=yes]
)

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

AM_CONDITIONAL([ENABLE_DOC], [test "x$sh_enable_doc" != xno])

AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x$sh_enable_examples" != xno])


dnl ****************************************************************************
dnl Check for Windows
dnl ****************************************************************************

AC_CANONICAL_HOST

case $host_os in
	*mingw*)
		sh_platform_mswin=yes
		sh_native_mswin=yes
		;;
	pw32* | *cygwin*)
		sh_platform_mswin=yes
		sh_native_mswin=no
		;;
	*)
		sh_platform_mswin=no
		sh_native_mswin=no
		;;
esac

AM_CONDITIONAL([PLATFORM_MSWIN], [test "x$sh_platform_mswin" = xyes])

AM_COND_IF([PLATFORM_MSWIN],
	[AC_CHECK_PROG([sh_have_windres],[windres],[yes],[no])],
	[sh_have_windres=no]
)

AM_CONDITIONAL([HAVE_WINDRES], [test "x$sh_have_windres" = xyes])

AM_CONDITIONAL([NATIVE_MSWIN], [test "x$sh_native_mswin" = xyes])

LT_INIT([win32-dll])

dnl ****************************************************************************
dnl End check for Windows
dnl ****************************************************************************


AC_SUBST([LIBRARY_NAME], [AC_LIBRARY_NAME])

AC_SUBST([LIBRARY_DESCRIPTION], [AC_LIBRARY_DESCRIPTION])

AC_CONFIG_FILES([
	Makefile
	src/]AC_PACKAGE_NAME[-]AC_PACKAGE_VERSION[.pc
	src/Makefile
])

AC_OUTPUT
     
