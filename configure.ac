dnl Process this file with autoconf to produce a configure script.

AC_INIT(libconfini, 1.7)

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--enable-doc], [Create package with documentation @<:@default=yes@:>@])],
	[:],
	[enable_doc=yes]
)

AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--enable-examples], [Create package with examples @<:@default=yes@:>@])],
	[:],
	[enable_examples=yes]
)

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

AM_CONDITIONAL([ENABLE_DOC], [test "$enable_doc" != "no"])

AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" != "no"])


dnl ***************************************************************************
dnl Check for Windows
dnl ***************************************************************************
AC_CANONICAL_HOST

case $host_os in
  *mingw*)
    platform_win32=yes
    native_win32=yes
    ;;
  pw32* | *cygwin*)
    platform_win32=yes
    native_win32=no
    ;;
  *)
    platform_win32=no
    native_win32=no
    ;;
esac

AM_CONDITIONAL(PLATFORM_WIN32, test x"$platform_win32" = "xyes")

AM_CONDITIONAL(NATIVE_WIN32, test x"$native_win32" = "xyes")

LT_INIT([win32-dll])

dnl ***************************************************************************
dnl End check for Windows
dnl ***************************************************************************


AC_OUTPUT([
Makefile
src/confini-1.7.pc
src/Makefile
])

