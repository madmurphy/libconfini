##  -*- Mode: M4; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-
##  Process this file with autoconf to produce a configure script.

###
###        /|/|
###       (_|_)      _ _ _                      __ _       _
###                 | (_) |__   ___ ___  _ __  / _(_)_ __ (_)
###                 | | | '_ \ / __/ _ \| '_ \| |_| | '_ \| |
###                 | | | |_) | (_| (_) | | | |  _| | | | | |
###                 |_|_|_.__/ \___\___/|_| |_|_| |_|_| |_|_|      _ _
###                                                               ( | )
###                                                               |/|/
###

m4_include([autostuff/m4/not-autotools.m4])

dnl  This package has not been tested with previous versions of GNU Autoconf...
AC_PREREQ([2.68])


###  **************************************************************************
###     P A C K A G E   M E T A D A T A
###  **************************************************************************

dnl  Please update the version numbers **only here**. All the other occurrences
dnl  within the package will be automatically updated by launching `./configure
dnl  --enable-extended-config`.
AC_INIT([lib]n4_mem([GL_PROJECT_NAME], [confini]),
	m4_joinall([.],
		n4_mem([GL_PROJECT_MAJVER], [1]),
		n4_mem([GL_PROJECT_MINVER], [16]),
		n4_mem([GL_PROJECT_REVVER], [0])),
	[madmurphy333@gmail.com],
	[lib]GL_PROJECT_NAME,
	[https://madmurphy.github.io/libconfini])

NC_GLOBAL_LITERALS(
	[INTERFACE_NUM],			[5],
	[IMPLEMENTATION_NUM],		[0],
	[INTERFACES_SUPPORTED],		[5],
	[PROJECT_DESCRIPTION],		[Yet another INI parser],
	[LICENSE_STRING],			[GNU General Public License, version 3 or any
								later version],
	[LICENSE_SPDX],				[GPL-3.0-or-later],
	[PUBLISHER_NAME],			[Stefano Gioffré],
	[PUBLISHER_ID],				[madmurphy],
	[GIT_HTTP_SERVER],			[https://github.com]
)


###  **************************************************************************
###     R E N A M I N G   R U L E S
###  **************************************************************************

NC_REQUIRE([AC_PROG_LN_S], [AC_PROG_SED])

AC_SUBST([PROJECT_NAME], [']GL_PROJECT_NAME['])

AS_IF([test "x${program_prefix}" != xNONE], [
	AS_VAR_SET([na_tmppref],
		["$(echo "${program_prefix}" | ${SED} \
		's/@<:@^@<:@:alnum:@:>@@:>@\+/-/g;s/@<:@A-Z@:>@/\L&/g;s/^-//')"])
	AS_IF([test "x${program_prefix}" != "x${na_tmppref}"],
		[AC_MSG_WARN(["${program_prefix}" is not a valid prefix: sanitized as "${na_tmppref}"])])
])

AS_IF([test "x${program_suffix}" != xNONE], [
	AS_VAR_SET([na_tmpsuff],
		["$(echo "${program_suffix}" | ${SED} \
		's/@<:@^@<:@:alnum:@:>@@:>@\+/-/g;s/@<:@A-Z@:>@/\L&/g;s/-@S|@//')"])
	AS_IF([test "x${program_suffix}" != "x${na_tmpsuff}"],
		[AC_MSG_WARN(["${program_suffix}" is not a valid suffix: sanitized as "${na_tmpsuff}"])])
])

AS_IF([echo 'x' | ${SED} "${program_transform_name}" 1>/dev/null 2>&1],
	[AS_VAR_COPY([na_tmptransf], [program_transform_name])],
	[AC_MSG_WARN([ignored --program-transform-name='${program_transform_name}' (invalid sed expression)'])])

AC_SUBST([PROJECT_CONFNAME],
	["$(echo "${na_tmppref}]GL_PROJECT_NAME[${na_tmpsuff}" | \
		sed "${na_tmptransf}")"])

AC_SUBST([PACKAGE_CONFNAME], ["lib${PROJECT_CONFNAME}"])

AC_ARG_WITH([other_versions],
	[AS_HELP_STRING([--with-other-versions],
		[make this package able to cohabit with other major versions of itself
		@<:@default=no@:>@])],
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
	[AC_MSG_NOTICE([renaming "]AC_PACKAGE_TARNAME[" to "${PACKAGE_CONFNAME}"...])])

AC_SUBST([PROJECT_LOCALNAME], ["${PROJECT_CONFNAME}${na_tmpvsuff}"])

AC_SUBST([PACKAGE_LOCALNAME], ["${PACKAGE_CONFNAME}${na_tmpvsuff}"])

AC_SUBST([library_headername], ["${PROJECT_CONFNAME}${na_tmphvsuff}"])

AM_COND_IF([HAVE_TRANSFORMATIONS],
	[AS_IF([NC_ARG_MISSING_WITHVAL([--docdir])],
		[AS_VAR_SET([docdir],
			['${datarootdir}/doc/'"${PACKAGE_LOCALNAME}"])])])

NS_UNSET([na_tmppref], [na_tmpsuff], [na_tmptransf], [na_tmpvsuff],
	[na_tmphvsuff])

###  Prevent any further transformations from Autoconf (unless
###  `AC_CANONICAL_TARGET` is invoked after these lines)

AS_VAR_SET([program_prefix], ['NONE'])

AS_VAR_SET([program_suffix], ['NONE'])

AS_VAR_SET([program_transform_name], ['s,x,x,'])


###  **************************************************************************
###     A U T O C O N F   E N V I R O N M E N T
###  **************************************************************************

###  Temporarily unset `CFLAGS` and let `AC_PROG_CC_C99` guess freely
NS_MOVEVAR([na_tmpusrcflags], [CFLAGS])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_AUX_DIR([build-aux])

###  Add `--enable-extended-config[=MODE]` option
NC_CONFIG_SHADOW_DIR([autostuff/shadows])

AM_INIT_AUTOMAKE([gnu])

###  Add `--enable-author-mode` option
AC_ARG_ENABLE([author-mode],
	[AS_HELP_STRING([--enable-author-mode],
		[enable make rules and dependencies that facilitate the programming
		workflow for developing ]AC_PACKAGE_NAME[ but are not useful (or are
        even counterproductive) to the casual installer
		@<:@default=<enable-extended-config>@:>@])],
	[AS_IF([test "x${enable_author_mode}" != xno],
		[AS_VAR_SET([enable_author_mode], ['yes'])],
		[AS_IF([test "x${enable_extended_config}" != xno], [
				AC_MSG_WARN([ignoring --disable-author-mode @{:@--enable-extended-config@:}@])
				AS_VAR_SET([enable_author_mode], ['yes'])
		])])],
	[AM_COND_IF([HAVE_EXTENDED_CONFIG],
		[AS_VAR_SET([enable_author_mode], ['yes'])],
		[AS_VAR_SET([enable_author_mode], ['no'])])])

AM_CONDITIONAL([AUTHOR_MODE], [test "x${enable_author_mode}" = xyes])

AM_MAINTAINER_MODE([enable])

AM_SILENT_RULES([yes])

AC_CANONICAL_HOST

AC_ARG_PROGRAM


###  **************************************************************************
###     C   C O M P I L E R   E N V I R O N M E N T
###  **************************************************************************

AC_PROG_CC_C99

AC_C_INLINE

AC_HEADER_STDBOOL

AC_CHECK_HEADERS([stddef.h stdint.h stdio.h stdlib.h])

AC_TYPE_SIZE_T

AC_FUNC_MALLOC

NC_CC_CHECK_POSIX

###  Save the guessed `CFLAGS` for `AM_CFLAGS` (see `src/Makefile.am`)
NC_SUBST_NOTMAKE([guessed_am_cflags], ["${CFLAGS}"])

###  Restore `CFLAGS` according to the user's will
NS_MOVEVAR([CFLAGS], [na_tmpusrcflags])

###  Add `--without-libc` option
AC_ARG_WITH([libc],
	[AS_HELP_STRING([--without-libc],
		[Assume that no C Standard Library is present in the system
		@<:@default=no@:>@])],
	[AS_IF([test "x${with_libc}" != xno],
		[AS_VAR_SET([with_libc], ['yes'])])],
	[AS_VAR_SET([with_libc], ['yes'])])

###  Add `--with-io-api=API` option
AC_ARG_WITH([io_api],
	[NA_HELP_STRINGS([[--with-io-api=API], [--without-io-api]],
		[build this package with a specific I/O API; possible values for API
		are: "standard" for using the C Standard I/O API, "posix" for using the
		POSIX I/O API, "baremetal" or "no" (--without-io-api) for using no I/O
		API at all, or "check" for automatically selecting the best I/O API
		supported by this machine @<:@default=check@:>@])],
	[AS_IF([test "x${withval}" = x -o "x${with_io_api}" = xyes], [
			AC_MSG_ERROR([an API value must be specified for option --with-io-api=API])
		], [test "x${with_io_api}" = xno], [
			AS_VAR_SET([na_tmp_ioflavor], [baremetal])
		], [test "x${with_libc}" = xno -a "x${with_val}" != xbaremetal], [
			AC_MSG_WARN([ignoring --with-io-api @{:@--without-libc@:}@])
			AS_VAR_SET([na_tmp_ioflavor], [baremetal])
		], [
			AS_VAR_COPY([na_tmp_ioflavor], [withval])
	])],
	[AS_IF([test "x${with_libc}" = xno],
		[AS_VAR_SET([na_tmp_ioflavor], ['baremetal'])],
		[AS_VAR_SET([na_tmp_ioflavor], ['check'])])])

AS_VAR_SET([na_cppenv], ['$(_with_io_cppenv_)'])

AS_CASE(["${na_tmp_ioflavor}"],
	[standard], [:],
	[posix], [
		AS_IF([test "x${ac_cv_have_posix_c}" = xno], [
			NC_MSG_WARNBOX([The POSIX I/O API is not supported on this platform
			(--with-io-api=posix). The C Standard I/O API will be used
			instead.])
			AS_VAR_SET([na_tmp_ioflavor], ['standard'])
		])
	],
	[baremetal], [
		AM_COND_IF([RENAME_PACKAGE],
			[AC_MSG_NOTICE([applying --without-io-api...])],
			[NC_MSG_WARNBOX([It is strongly recommended to rename the package
			when compiling it without a I/O API (--without-io-api). For
			example, to rename `]AC_PACKAGE_TARNAME[` to
			`lib]GL_PROJECT_NAME[-baremetal`, use `./configure --without-io-api
			--program-suffix='-baremetal'`. Type `./configure --help` for more
			information about renaming rules.])])
		AS_VAR_SET([na_want_baremetal], ['yes'])
		AS_VAR_SET([na_tmp_ioflavor], ['standard'])
		AS_VAR_SET([na_cppenv], ['$(_baremetal_cppenv_)'])
		AS_VAR_SET([na_tmp_further_pp_macros],
			[' '\''-D_LIBCONFINI_COMPILE_HEADER_="confini-baremetal.h"'\'])
	],
	[check], [
		AS_IF([test "x${ac_cv_have_posix_c}" = xyes],
			[AS_VAR_SET([na_tmp_ioflavor], ['posix'])],
			[AS_VAR_SET([na_tmp_ioflavor], ['standard'])])
	],
	[*], [
		AC_MSG_ERROR([unrecognized option: --with-io-api=${na_tmp_ioflavor}])
	])

AM_CONDITIONAL([WANT_BAREMETAL], [test "x${na_want_baremetal}" = xyes])

NC_SUBST_NOTMAKE([na_io_api],
	["CONFINI_$(echo "${na_tmp_ioflavor}" | tr '[a-z]' '[A-Z]')"])

NC_SUBST_NOTMAKE([na_cppenv])

NS_UNSET([na_tmp_further_pp_macros], [na_tmp_ioflavor])

AS_MKDIR_P(['no-dist/conf-results'])

AS_MKDIR_P(['no-dist/hackings/baremetal'])

AC_SUBST([rvdir], ['REVISEME'])

AC_SUBST([nonportfile], ['non-portable.h'])

###  When compiling without a I/O API (--without-io-api), try to guess a
###  possible re-implementation of the required standard data types if these
###  are missing...
AM_COND_IF([WANT_BAREMETAL], [
	AS_IF([NS_TEST_AEQ([yes],
	[${with_libc}], [${ac_cv_header_stddef_h}],
	[${ac_cv_header_stdbool_h}], [${ac_cv_header_stdint_h}],
	[${ac_cv_type_size_t}])], [
		AS_VAR_SET([na_revise_libc], ['no'])
	], [
		AS_VAR_SET([na_revise_libc], ['yes'])
		AS_MKDIR_P(["${rvdir}"])
		NC_CC_CHECK_SIZEOF([unsigned short int], [], [ushort])
		NC_CC_CHECK_SIZEOF([unsigned int], [], [uint])
		NC_CC_CHECK_SIZEOF([unsigned long int], [], [ulong])
		NC_CC_CHECK_SIZEOF([sizeof(char)], [], [size_t])
		cp "${srcdir}/autostuff/chunks/revision-warning.c" \
			'no-dist/conf-results/stdmissing.h'
		rm -f 'no-dist/conf-results/noioheaders.h'
		AS_IF([NS_TEST_AEQ([yes],
		[${with_libc}], [${ac_cv_header_stddef_h}],
		[${ac_cv_type_size_t}])],
			[echo '@%:@include <stddef.h>' >> \
				'no-dist/conf-results/noioheaders.h'],
			[{
				# `size_t`...
				echo '/*  Partial re-implementation of `stddef.h`  */'
				AS_IF([test "${ac_cv_sizeof_size_t}" -eq 1],
					[echo 'typedef unsigned char size_t;'],
				[test "${ac_cv_sizeof_ushort}" -ge "${ac_cv_sizeof_size_t}"],
					[echo 'typedef unsigned short int size_t;'],
				[test "${ac_cv_sizeof_uint}" -ge "${ac_cv_sizeof_size_t}"],
					[echo 'typedef unsigned int size_t;'],
				[test "${ac_cv_sizeof_ulong}" -ge "${ac_cv_sizeof_size_t}"],
					[echo 'typedef unsigned long int size_t;'],
					[echo 'typedef unsigned long long int size_t;'])
			} >> 'no-dist/conf-results/stdmissing.h'])
		AS_IF([NS_TEST_AEQ([yes],
		[${with_libc}], [${ac_cv_header_stdbool_h}])],
			[echo '@%:@include <stdbool.h>' >> \
				'no-dist/conf-results/noioheaders.h'],
			[{
				# `bool`...
				echo '/*  Re-implementation of `stdbool.h`  */'
				echo '@%:@ifndef bool'
				echo '@%:@define bool unsigned char'
				echo '@%:@define false 0'
				echo '@%:@define true 1'
				echo '@%:@endif'
			} >> 'no-dist/conf-results/stdmissing.h'])
		AS_IF([NS_TEST_AEQ([yes],
		[${with_libc}], [${ac_cv_header_stdint_h}])], [
			echo '@%:@include <stdint.h>' >> \
				'no-dist/conf-results/noioheaders.h'
		], [
			NC_CC_CHECK_CHAR_BIT
			AS_VAR_SET([na_tmp_ushort_bit],
				["$(expr "${ac_cv_sizeof_ushort}" '*' "${ac_cv_char_bit}")"])
			AS_VAR_SET([na_tmp_uint_bit],
				["$(expr "${ac_cv_sizeof_uint}" '*' "${ac_cv_char_bit}")"])
			{
				# `uint8_t`, `int8_t`, `uint16_t`, `uint32_t`...
				echo '/*  Partial re-implementation of `stdint.h`  */'
				# `unsigned char` is guaranteed to be at least 8-bit...
				AS_IF([NS_TEST_NAE([yes],
				[${with_libc}], [${ac_cv_c_uint8_t}])],
					[echo 'typedef unsigned char uint_least8_t;'])
				# `signed char` is guaranteed to be at least 8-bit...
				AS_IF([NS_TEST_NAE([yes],
				[${with_libc}], [${ac_cv_c_int8_t}])],
					[echo 'typedef signed char int_least8_t;'])
				# `unsigned int` is guaranteed to be at least 16-bit...
				AS_IF([NS_TEST_NAE([yes],
				[${with_libc}], [${ac_cv_c_uint16_t}])],
					[AS_IF([test "${ac_cv_char_bit}" -ge 16],
						[echo 'typedef unsigned char uint_least16_t;'],
					[test "${na_tmp_ushort_bit}" -ge 16],
						[echo 'typedef unsigned short int uint_least16_t;'],
						[echo 'typedef unsigned int uint_least16_t;'])])
				# `unsigned long int` is guaranteed to be at least 32-bit...
				AS_IF([NS_TEST_NAE([yes],
				[${with_libc}], [${ac_cv_c_uint32_t}])],
					[AS_IF([test "${ac_cv_char_bit}" -ge 32],
						[echo 'typedef unsigned char uint_least32_t;'],
					[test "${na_tmp_ushort_bit}" -ge 32],
						[echo 'typedef unsigned short int uint_least32_t;'],
					[test "${na_tmp_uint_bit}" -ge 32],
						[echo 'typedef unsigned int uint_least32_t;'],
					[echo 'typedef unsigned long int uint_least32_t;'])])
			} >> 'no-dist/conf-results/stdmissing.h'
		])
		NS_UNSET([na_tmp_ushort_bit], [na_tmp_uint_bit])
		AS_IF([test -f 'no-dist/conf-results/noioheaders.h'],
			[echo >> 'no-dist/conf-results/noioheaders.h'])
		AS_IF([test -f "${rvdir}/${nonportfile}"], [
			touch "${rvdir}/${nonportfile}"
			AS_VAR_SET([na_tmp_pending_rv], ['yes'])
		], [
			cp 'no-dist/conf-results/stdmissing.h' "${rvdir}/${nonportfile}"
		])
	])
], [
	AS_VAR_SET([na_revise_libc], ['no'])
])

AM_CONDITIONAL([REVISE_LIBC], [test "x${na_revise_libc}" = xyes])

AM_COND_IF([REVISE_LIBC],
	[AS_VAR_SET([revision_clean_recipe],
		m4_normalize(['test -f '\''$(rvdir)/$(nonportfile)'\'' ||
			$(MAKE) '\''$(rvdir)/$(nonportfile)'\'';']))],
	[cp "${srcdir}/dev/hackings/baremetal/c-standard-library.h" \
		'no-dist/hackings/baremetal/c-standard-library.h'])

NC_SUBST_NOTMAKE([revision_clean_recipe])

###  Add `TESTS_PRINTF_HEADER` argument variable
AC_ARG_VAR([TESTS_PRINTF_HEADER],
	[The C header file containing the `printf()` function - this variable
	applies to checks only and does **not** apply to the library's code
	@<:@default='<stdio.h>'@:>@])

###  Add `TESTS_PRINTF_FUNCTION` argument variable
AC_ARG_VAR([TESTS_PRINTF_FUNCTION],
	[An alternative name for the `printf()` function - this variable applies to
	checks only and does **not** apply to the library's code
	@<:@default=printf@:>@])

AC_SUBST([TESTSONLY_CPPFLAGS],
	m4_normalize([
		"$(test "x${TESTS_PRINTF_HEADER}" = x || echo '
		"-DTESTS_PRINTF_HEADER=$(TESTS_PRINTF_HEADER)"')$(test
		"x${TESTS_PRINTF_FUNCTION}" = x || echo '
		"-DTESTS_PRINTF_FUNCTION=$(TESTS_PRINTF_FUNCTION)"')"
	]))


###  **************************************************************************
###     ( A U T O ) M A K E   E N V I R O N M E N T
###  **************************************************************************

###  Add `RC` argument variable
AC_ARG_VAR([RC], [Microsoft Windows resource compiler command])

###  Add `RCFLAGS` argument variable
AC_ARG_VAR([RCFLAGS], [Microsoft Windows resource compiler flags])

NC_REQ_PROGS([find], [Unix find utility])

###  Keep the `pkgconfigdir` environment variable when non-empty
AS_IF([test "x${pkgconfigdir}" = x],
	[AS_VAR_SET([pkgconfigdir], ['$(libdir)/pkgconfig'])])

###  Add `--with-pkgconfigdir=DIR` option
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

###  Add `--enable-version-info` and `--disable-version-info` options
AC_ARG_ENABLE([version-info],
	[NA_HELP_STRINGS([[--enable-version-info], [--disable-version-info]],
		[enable/disable libtool versioning system
		@<:@default=SYSTEM-DEPENDENT@:>@])],
	[:],
	[AS_VAR_SET([enable_version_info], ['check'])])

###  Add `--disable-devel` option
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

###  Add `--disable-doc` option
AC_ARG_ENABLE([doc],
	[AS_HELP_STRING([--disable-doc],
		[don't generate any documentation @<:@default=<disable-devel>@:>@])],
	[AS_IF([test "x${enable_devel}" = xno -a "x${enable_doc}" != xno], [
		AC_MSG_WARN([ignoring --enable-doc @{:@--disable-devel@:}@])
		AS_VAR_SET([enable_doc], ['no'])
	])],
	[AS_VAR_COPY([enable_doc], [enable_devel])])

###  Add `--disable-man` option
AC_ARG_ENABLE([man],
	[AS_HELP_STRING([--disable-man],
		[don't generate man pages @<:@default=<disable-doc>@:>@])],
	[AS_IF([test "x${enable_doc}" = xno -a "x${enable_man}" != xno], [
		AC_MSG_WARN([ignoring --enable-man @{:@--disable-doc@:}@])
		AS_VAR_SET([enable_man], ['no'])
	])],
	[AS_VAR_COPY([enable_man], [enable_doc])])

###  Add `--enable-examples` and `--disable-examples` options
AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples],
		[don't generate examples @<:@default=<disable-doc/without-io-api>@:>@])],
	[AS_IF([test "x${enable_devel}" = xno -a "x${enable_examples}" != xno], [
		AC_MSG_WARN([ignoring --enable-examples @{:@--disable-devel@:}@])
		AS_VAR_SET([enable_examples], ['no'])
	])],
	[AM_COND_IF([WANT_BAREMETAL],
		[AS_VAR_SET([enable_examples], ['no'])],
		[AS_VAR_COPY([enable_examples], [enable_doc])])])

###  Add `--enable-htmldoc` option
AC_ARG_ENABLE([htmldoc],
	[AS_HELP_STRING([--enable-htmldoc],
		[generate the html documentation @<:@default=no@:>@])],
	[AS_IF([test "x${enable_doc}" = xno -a "x${enable_htmldoc}" != xno], [
		AC_MSG_WARN([ignoring --enable-htmldoc @{:@--disable-doc@:}@])
		AS_VAR_SET([enable_htmldoc], ['no'])
	])],
	[AS_VAR_SET([enable_htmldoc], ['no'])])

AM_CONDITIONAL([WANT_DEVEL], [test "x${enable_devel}" != xno])

AM_CONDITIONAL([WANT_DOC], [test "x${enable_doc}" != xno])

AM_CONDITIONAL([WANT_MAN], [test "x${enable_man}" != xno])

AM_CONDITIONAL([WANT_EXAMPLES], [test "x${enable_examples}" != xno])

AM_CONDITIONAL([WANT_HTML], [test "x${enable_htmldoc}" != xno])

AS_IF([test "x${na_want_baremetal}" = xyes -a "x${enable_examples}" != xno],
	[NC_MSG_WARNBOX([Many examples will not work without a I/O API
	@{:@--enable-examples --without-io-api@:}@])])

dnl  PREPARE_MAN3PAGES(page1, renamed-page1[, ... pageN, renamed-pageN])
dnl  **************************************************************************
dnl
dnl  Apply renamings (if any) to man pages and set the `MAN3PAGES` substitution
dnl
AC_DEFUN_ONCE([PREPARE_MAN3PAGES], [
	AM_COND_IF([WANT_DOC], [
		AS_IF([test -d 'no-dist/man3'],
			[rm -r 'no-dist/man3'])
		AS_MKDIR_P(['no-dist/man3'])
		AM_COND_IF([HAVE_TRANSFORMATIONS], [
			m4_for([__i__], [1], [$#], [2], [
				cp "${srcdir}/docs/man/man3/m4_normalize(m4_argn(__i__, $@))" \
					"no-dist/man3/m4_normalize(m4_argn(m4_incr(__i__), $@))";
			])
		], [
			m4_for([__i__], [1], [$#], [2], [
				cp "${srcdir}/docs/man/man3/m4_normalize(m4_argn(__i__, $@))" \
					'no-dist/man3/';
			])
		])
		AS_VAR_SET([MAN3PAGES], ["$(echo no-dist/man3/*)"])
	])
	AC_SUBST([MAN3PAGES])
])

dnl  Man pages go here, each page accompanied by a safe cohabitation renaming
dnl  and expected to be found in `${srcdir}/docs/man/man3`
PREPARE_MAN3PAGES(
	AC_PACKAGE_TARNAME[.3],	[${PACKAGE_LOCALNAME}.3],
	GL_PROJECT_NAME[.h.3],	[${library_headername}.h.3],
	[IniStatistics.3],		[${PACKAGE_LOCALNAME}-IniStatistics.3],
	[IniFormat.3],			[${PACKAGE_LOCALNAME}-IniFormat.3],
	[IniDispatch.3],		[${PACKAGE_LOCALNAME}-IniDispatch.3]
)


###  **************************************************************************
###     P L A T F O R M   C H E C K S
###  **************************************************************************

AS_CASE(["${host_os}"],
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

AC_SUBST([host_cpu_pretty], ["$(echo "${host_cpu}" | tr '_' '-')"])


###  **************************************************************************
###     L I B T O O L
###  **************************************************************************

LT_INIT([win32-dll])

AM_COND_IF([PLATFORM_MSWIN], [
	AC_CONFIG_FILES([src/winres-ac.rc:]NC_SHADOW_DIR[/src/winres-ac.rc.in])
	LT_PROG_RC
])

# `LT_INIT([win32-dll])` should set the `${DLLTOOL}` variable
AM_CONDITIONAL([HAVE_DLLTOOL],
	[test "x${DLLTOOL}" != xfalse -a "x${DLLTOOL}" != x])

# `LT_PROG_RC` should set the `${RC}` variable
AM_CONDITIONAL([HAVE_RC],
	[test "x${RC}" != xfalse -a "x${RC}" != x])

###  Add `--enable-dllexp` option
AC_ARG_ENABLE([dllexp],
	[AS_HELP_STRING([--enable-dllexp],
		[create module-definition (.def) and export (.exp) files when compiling
		for Microsoft Windows @<:@default=no@:>@])],
	[AS_IF([test "x${enable_dllexp}" != xno],
		[AM_COND_IF([HAVE_DLLTOOL], [
				AS_VAR_SET([enable_dllexp], ['yes'])
			], [
				AC_MSG_WARN([dlltool not available, ignoring --enable-dllexp])
				AS_VAR_SET([enable_dllexp], ['no'])
		])])],
	[AS_VAR_SET([enable_dllexp], ['no'])])

AM_CONDITIONAL([WANT_DLLEXP], [test "x${enable_dllexp}" != xno])


###  **************************************************************************
###     F I N E   T U N I N G
###  **************************************************************************

###  **************************************************************************
###     TODO:  For a perfect optimization it would be needed to test `-O2`
###            and `-O3` under more conditions...
###  **************************************************************************

###  Optimization `-O3` seems to be working better than `-O2` when compiling
###  under native Microsoft Windows, in all other cases use `-O2`. Users can
###  always override the default value by passing a `CFLAGS` variable.
AM_COND_IF([NATIVE_MSWIN],
	[AS_VAR_SET([default_optimization], [3])],
	[AS_VAR_SET([default_optimization], [2])])

AC_SUBST([default_optimization])


###  **************************************************************************
###     S U B S T I T U T I O N S
###  **************************************************************************

AC_CONFIG_FILES([
	Makefile
	src/${PACKAGE_LOCALNAME}.pc:src/]AC_PACKAGE_TARNAME[.pc.in
	src/Makefile
	tests/Makefile
])

NC_THREATEN_FILES(
	[package.json],
	[src/winres.rc],
	[dev/hackings/all/confini-metadata.c],
	[dev/hackings/all/confini-metadata.h],
)

AC_CONFIG_LINKS([help/INSTALL.md:INSTALL])

###  More renaming and extended config rules...
AM_COND_IF([HAVE_EXTENDED_CONFIG], [
	AM_COND_IF([RENAME_PACKAGE], [
		AS_VAR_SET([transf_dist_recipe],
			m4_normalize([']m4_foreach([__i__], [NC_THREATENED_LIST], [[
				! test -f '\''$(srcdir)/'"]__i__["'.dist'\'' ||
					cp '\''$(srcdir)/'"]__i__["'.dist'\''
					'\''$(distdir)/'"]__i__["\'';
			]])[']))
	])
	AM_COND_IF([HAVE_UPDATES], [
		AM_COND_IF([RENAME_PACKAGE], [
			AS_VAR_SET([mergenew_recipe],
				m4_normalize([
					'echo '\''**Warning** Extended configuration has been
					called on a renamed package. All'\'' && echo '\''updates
					will be reverted with `make clean` and will not be
					re-distributed with'\'' && echo '\''`make dist`!'\'';
					]m4_foreach([__i__], [NC_THREATENED_LIST], [[
					test -f '\''$(srcdir)/'"]__i__["'.dist'\'' ||
						cp '\''$(srcdir)/'"]__i__["\''
						'\''$(srcdir)/'"]__i__["'.dist'\'';
					cp '\''$(confnewdir)/'"]__i__["\''
						'\''$(srcdir)/'"]__i__["\'' &&
						echo '\''File $(srcdir)/'"]__i__["'
						successfully updated.'\'' || echo '\''File
						$(confnewdir)/'"]__i__["' not found.'\'';
				]])[']))
			# The the `.dist` files have been generated by `make synch`, hence
			# `make clean` must restore them
			AS_VAR_SET([transf_clean_recipe],
				m4_normalize([']m4_foreach([__i__], [NC_THREATENED_LIST], [[
					! test -f '\''$(srcdir)/'"]__i__["'.dist'\'' ||
						mv '\''$(srcdir)/'"]__i__["'.dist'\''
						'\''$(srcdir)/'"]__i__["\'';
				]])[']))
		], [
			AS_VAR_SET([mergenew_recipe],
				m4_normalize([']m4_foreach([__i__], [NC_THREATENED_LIST], [[
					test -f '\''$(confnewdir)/'"]__i__["\'' &&
						cp '\''$(confnewdir)/'"]__i__["\''
						'\''$(srcdir)/'"]__i__["\'' &&
						echo '\''File $(srcdir)/'"]__i__["' successfully
						updated.'\'' || echo '\''File $(confnewdir)/'"]__i__["'
						not found.'\'';
				]])[']))
		])
	], [
		AS_VAR_SET([mergenew_recipe],
			m4_normalize([
				'echo '\''Extended configuration had been already merged with
				the package tree (for more'\'' && echo '\''information about
				the updates see the content of `./$(confnewdir)`).'\'
		]))
		AM_COND_IF([RENAME_PACKAGE], [
			NC_MSG_WARNBOX([Extended configuration has been called on a renamed
			package. Changes will be reverted with `make distclean` and will
			not be re-distributed with `make dist`!])
			m4_foreach([__i__], [NC_THREATENED_LIST],
				[test -f "${srcdir}/]__i__[.dist" || \
					cp "${srcdir}/]__i__[" "${srcdir}/]__i__[.dist";])
			# The the `.dist` files have been generated by the `configure`
			# script, hence `make distclean` must restore them
			AS_VAR_SET([transf_distclean_recipe],
				m4_normalize([']m4_foreach([__i__], [NC_THREATENED_LIST], [[
					! test -f '\''$(srcdir)/'"]__i__["'.dist'\'' ||
						mv '\''$(srcdir)/'"]__i__["'.dist'\''
						'\''$(srcdir)/'"]__i__["\'';
				]])[']))
		])
	])
], [
	AS_VAR_SET([mergenew_recipe],
		m4_normalize([
			'echo '\''Extended configuration is currently disabled.'\'
		]))
])

NC_SUBST_NOTMAKE([transf_dist_recipe])

NC_SUBST_NOTMAKE([transf_clean_recipe])

NC_SUBST_NOTMAKE([transf_distclean_recipe])

NC_SUBST_NOTMAKE([mergenew_recipe])

AC_SUBST([lt_cv_objdir])

AC_SUBST([PROJECT_MAJVER], [']GL_PROJECT_MAJVER['])

AC_SUBST([PROJECT_MINVER], [']GL_PROJECT_MINVER['])

AC_SUBST([PROJECT_REVVER], [']GL_PROJECT_REVVER['])

AC_SUBST([PROJECT_EFFVER], [']GL_PROJECT_MAJVER[.]GL_PROJECT_MINVER['])

AC_SUBST([LIBRARY_BINVERSION],
	m4_joinall([.],
		m4_eval(GL_INTERFACE_NUM[ - ]GL_INTERFACES_SUPPORTED),
		GL_INTERFACES_SUPPORTED,
		GL_IMPLEMENTATION_NUM))


###  **************************************************************************
###     R E B U I L D   T H E   M A I N   S O U R C E   C O D E
###  **************************************************************************

##  src/Makefile.am, `make confini.c`
###  No amendments here, the marked files already contain the official version
NC_SUBST_NOTMAKE([confini_c_recipe],
	m4_normalize([
		']m4_quote(NA_TRIANGLE_BRACKETS_TO_MAKE_VARS(m4_quote(
		NA_DOUBLE_DOLLAR(m4_quote(NA_ESC_APOS(m4_quote(NA_AMEND([<@>],
			[<top_srcdir>/dev/marked-sources/confini-marked.c],
			[[CONFINI_C_METADATA],
				[<top_srcdir>/dev/hackings/all/confini-metadata.c]]))))))))[;'
	]))

##  src/Makefile.am, `make confini.h`
###  No amendments here, the marked files already contain the official version
NC_SUBST_NOTMAKE([confini_h_recipe],
	m4_normalize([
		']m4_quote(NA_TRIANGLE_BRACKETS_TO_MAKE_VARS(m4_quote(
		NA_DOUBLE_DOLLAR(m4_quote(NA_ESC_APOS(m4_quote(NA_AMEND([<@>],
			[<top_srcdir>/dev/marked-sources/confini-marked.h],
			[[CONFINI_H_METADATA],
				[<top_srcdir>/dev/hackings/all/confini-metadata.h]]))))))))[;'
	]))


###  **************************************************************************
###     H A C K I N G S
###  **************************************************************************

##  src/Makefile.am, `make confini-baremetal.c`
NC_SUBST_NOTMAKE([confini_baremetal_c_recipe],
	m4_normalize([
		'@S|@@{:@MKDIR_P@:}@ '\''@S|@@{:@@D@:}@'\'' &&
		]NA_ESC_APOS(m4_quote(NA_TRIANGLE_BRACKETS_TO_MAKE_VARS(m4_quote(
		NA_DOUBLE_DOLLAR(m4_quote(NA_AMEND([<@>],
			[<top_srcdir>/dev/marked-sources/confini-marked.c],
			[[CONFINI_C_METADATA],
				[<top_srcdir>/dev/hackings/all/confini-metadata.c]],
			[[IO_FLAVORS],
				[]],
			[[STANDARD_HEADERS],
				[]],
			[[CONFINI_HEADER],
				[<top_srcdir>/dev/hackings/baremetal/confini-header.c]],
			[[IO_ALIASES],
				[]],
			[[IO_FUNCTIONS],
				[]],
			[[LIBC_STR2NUM],
				[<top_srcdir>/dev/hackings/baremetal/str2num.c]])))))))[
			;'
	]))

##  src/Makefile.am, `make confini-baremetal.h`
NC_SUBST_NOTMAKE([confini_baremetal_h_recipe],
	m4_normalize([
		"@S|@@{:@test "x${na_revise_libc}" != xyes || echo 'test
		-e '\''../no-dist/hackings/baremetal/c-standard-library.h'\'' && @{:@
		'@:}@"'@S|@@{:@MKDIR_P@:}@ '\''@S|@@{:@@D@:}@'\'' &&
		]NA_ESC_APOS(m4_quote(NA_TRIANGLE_BRACKETS_TO_MAKE_VARS(m4_quote(
		NA_DOUBLE_DOLLAR(m4_quote(NA_AMEND([<@>],
			[<top_srcdir>/dev/marked-sources/confini-marked.h],
			[[CONFINI_H_METADATA],
				[<top_srcdir>/dev/hackings/all/confini-metadata.h]],
			[[STANDARD_HEADERS],
				[<top_builddir>/no-dist/hackings/baremetal/c-standard-library.h]],
			[[IO_FUNCTIONS],
				[]],
			[[LIBC_STR2NUM],
				[<top_srcdir>/dev/hackings/baremetal/str2num.h]])))))))[
		'"@S|@@{:@test "x${na_revise_libc}" != xyes || echo '@:}@ ||
		@{:@echo >&2 && { echo '\''I am not ready to build `@S|@@{:@D@:}@` yet.
		Please revise the content of the `@S|@@{:@rvdir@:}@`'\''; echo
		'\''subdirectory, then launch `@S|@@{:@MAKE@:}@ approve-revision` after
		you are done. If you'\''; echo '\''happen to change your mind, launch
		`@S|@@{:@MAKE@:}@ reject-revision` to go back.'\''; echo; } >&2 && exit
		2@:}@'@:}@"';'
	]))


###  **************************************************************************
###     O U T P U T
###  **************************************************************************

AC_OUTPUT

NC_SHADOW_AFTER_OUTPUT([
	AC_MSG_NOTICE([updating the source code with `${ac_make} all-official-sources`...])
	"${ac_make}" all-official-sources
])


###  **************************************************************************
###     H E L P   A N D   F I N A L   M E S S A G E S
###  **************************************************************************

AM_COND_IF([REVISE_LIBC],
	[AS_IF([test "x${na_tmp_pending_rv}" = xyes], [
		NC_MSG_WARNBOX([Do not forget to revise the content of the `${rvdir}`
		subdirectory before proceeding (--without-io-api).])
	], [test "x${with_libc}" = xyes], [
		NC_MSG_WARNBOX([I could not access one or more C standard objects. I am
		assuming that you are compiling for some exotic platform
		(--without-io-api), therefore I have tried to guess a possible
		re-implementation of the standard data types needed to build
		]AC_PACKAGE_NAME[ for your machine. You will need to review the guessed
		content in the `${rvdir}` subdirectory before launching `make all`.
		Launch `make approve-revision` after you are done. If you happen to
		change your mind, launch `make reject-revision` to go back.])
	], [
		NC_MSG_WARNBOX([The C Standard Library has been disabled
		(--without-libc), therefore I have tried to guess a possible
		re-implementation of the standard data types needed to build
		]AC_PACKAGE_NAME[ for your machine. You will need to review the guessed
		content in the `${rvdir}` subdirectory before launching `make all`.
		Launch `make approve-revision` after you are done. If you happen to
		change your mind, launch `make reject-revision` to go back.])
	])])

AS_UNSET([na_tmp_pending_rv])

###  Create the `help` subdirectory
AS_MKDIR_P(['help'])

###  Create a `help/make-targets.txt` file to be printed by `make help`
{ cat << 'END_HEREDOC'
GNU make utility to maintain AC_PACKAGE_STRING

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
		[delete all the files created by `make install(-*)`],
	[[make check], [make installcheck], [make finishcheck]],
		[check that the compiled library is in good conditions; in particular,
		`make check` tests the compiled objects directly from the source
		directory, `make installcheck` tests the compiled objects from the
		installation paths (you can pass a `DESTDIR` variable in case of staged
		installations), and `make finishcheck` tests the compiled objects from
		the installation paths without passing any information to the compiler
		about the latter, in order to check that <includedir> and <libdir> are
		automatically reachable (therefore no `DESTDIR` variable is supported
		here); note that `make installcheck` and `make finishcheck` can be
		launched only after having launched `make install(*)`],
	[[make mostlyclean], [make clean], [make distclean]],
		[clean the source directory according to different degrees of fury],
	[[make clobber]],
		[launch `make uninstall` followed by `make clean`],
	[[make alldone]],
		[launch `make install-strip` followed by `make installcheck` - in case
		of staged installations it is possible to pass a `DESTDIR` variable (see
		below)],
	[[make binary-release], [make manifest]],
		[create respectively a package and a manifest in the current location
		containing all the files that would be installed by `make
		install-strip` - if `make (all)` has not been launched yet, it will be
		launched first],
	[[make portable-builds]],
		[create a package containing all the files that would be installed by
		`make install-exec-strip`, but without any directory structure - if
		`make (all)` has not been launched yet, it will be launched first],
	[[make git-clone]],
		[download the latest revision of ]AC_PACKAGE_NAME[ from the server (the
		`git` utility is required); note that the downloaded archive might
		be a work in progress and not an official release yet]
)

Package maintenance targets:

NA_HELP_STRINGS(
	[[make dist(-*)], [make source-release], [make authors-copy]],
		[create a re-distributable copy of this package, ignoring any local
		configuration; the `make dist(*)` targets are GNU Automake's built-in
		recipes for redistributing the source code of a generic package, while
		the last two targets have been written with this package in mind (but
		they all do their job well); in particular, `make source-release` is
		similar to `make dist`, with the difference that it tries to reproduce
		the shape of the package as close as possible to the original release;
		same goes for `make authors-copy`, although this goes a step further
		and re-creates the original package as after launching `make
		bootstrap-clean`],
	[[make maintainer-clean], [make bootstrap-clean], [make oblivion-clean]],
		[clean the source directory according to different degrees of fury
		beyond `make distclean` - unless you are a package maintainer you
		should not use these targets, as your machine might not have the tools
		for re-creating the files deleted by `make maintainer-clean`, `make
		bootstrap-clean` and `make oblivion-clean`],
	[[make distcheck]],
		[create a redistributable package and check that everything in it
		works as it is supposed to work],
	[[make snapshot]],
		[create a clean backup of everything, including non-distributable
		files, but without the files generated by GNU Autotools]
)

Package development targets:

NA_HELP_STRINGS(
	[[make mergenew]],
		[merge all the files created by the `configure` option
		`--enable-extended-config=sandbox`],
	[[make all-baremetal-sources], [make baremetal-source-code],
	[make baremetal-symbols], [make all-official-sources],
	[make official-source-code], [make official-symbols], [make all-sources]],
		[(Re-)generate the source code of ]AC_PACKAGE_NAME[ using the content
		of the `dev` subdirectory these targets are available only after
		passing the `--enable-author-mode` option to the `configure` script]
)

Special targets:

NA_HELP_STRINGS(
	[[make all-baremetal-sources], [make baremetal-source-code],
	[make baremetal-symbols]],
		[Generate the "bare metal" source code of ]AC_PACKAGE_NAME[ - these
		targets are available only after passing the `--without-io-api` option
		to the `configure` script],
	[[make approve-revision], [make reject-revision]],
		[Approve/reject the current customization for a non-standard
		environment (available only in specific cases)],
	[[make refresh]],
		[(Re)build all the targets listed in the `EXPIRE` user variable, as in
		`make EXPIRE=']AC_PACKAGE_TARNAME[-git.tar.gz
		]AC_PACKAGE_TARNAME[-snapshot.tar.gz' refresh`]
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
		contain absolute paths only and defaults to an empty string when
		omitted; the `DESTDIR` variable is meant to be used only in case of
		staged installations and should never contain a real installation path
		- if you need to customize the target directories, launch `./configure
		--help`],
	[[ZIPTYPE]],
		[Supported by `make authors-copy`, `make binary-release`, `make
		git-clone`, `make portable-builds`, `make source-release` and `make
		snapshot`, this variable allows to change the compression format of the
		archives created; possible values are: `tar.gz`, `tar.xz`, `tgz`,
		`txz`, `zip`],
	[[DEST]],
		[Supported by `make authors-copy`, `make binary-release`, `make
		git-clone`, `make manifest`, `make portable-builds` and `make
		source-release`, the `DEST` variable allows to change the destination
		and possibly the compression format of the archives created (in the
		last case with the exception of `make manifest`, as this never produces
		archives); for example: `make DEST=']AC_PACKAGE_TARNAME[-edited.zip'
		portable-builds` creates a zip archive named
		`]AC_PACKAGE_TARNAME[-edited` instead of the default `.tar.gz` archive;
		if no recognizable extension is present, a directory will be created
		instead; archive formats supported are: `tar.gz`, `tar.xz`, `tgz`,
		`txz`, `zip`; this variable always overrides `ZIPTYPE`],
	[[DISTFLAGS]],
		[Supported by `make source-release` and `make authors-copy`, the
		`DISTFLAGS` variable can contain a space-separated list of options for
		the generated package. The only flag currently supported is
		`no-symlinks`, which can be used for replacing all the symbolic links
		present in the package with hard copies],
	[[EXPIRE]],
		[A space-separated list of targets to build even when up-to-date]
)

For further options, please refer to the documentation of GNU Automake.

Report bugs to <AC_PACKAGE_BUGREPORT>.
AC_PACKAGE_NAME home page: <AC_PACKAGE_URL>.
END_HEREDOC

AS_ECHO("This package is currently configured for a ${host_cpu_pretty} CPU running ${host_os}.")

} > 'help/make-targets.txt'

###  Save the output of `./configure --help`
{ "${0}" --help; echo; } > 'help/configure-options.txt'

AC_MSG_NOTICE([all done, launch `${ac_make} help` to list the commonly used targets.])


dnl  **************************************************************************
dnl     NOTE:  About the macros whose names begin with `GL_`, `NA_`, `NC_`,
dnl            `NM_`, `NS_`, `n4_` and what these prefixes are, see
dnl            `autostuff/m4/not-autotools.m4`.
dnl  **************************************************************************


dnl  EOF

