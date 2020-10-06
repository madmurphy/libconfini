dnl  -*- Mode: M4; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-

dnl  **************************************************************************
dnl         _   _       _      ___        _        _              _     
dnl        | \ | |     | |    / _ \      | |      | |            | |    
dnl        |  \| | ___ | |_  / /_\ \_   _| |_ ___ | |_ ___   ___ | |___ 
dnl        | . ` |/ _ \| __| |  _  | | | | __/ _ \| __/ _ \ / _ \| / __|
dnl        | |\  | (_) | |_  | | | | |_| | || (_) | || (_) | (_) | \__ \
dnl        \_| \_/\___/ \__| \_| |_/\__,_|\__\___/ \__\___/ \___/|_|___/
dnl
dnl            A collection of useful m4-ish macros for GNU Autotools
dnl
dnl                                               -- Released under GNU GPL3 --
dnl
dnl                                  https://github.com/madmurphy/not-autotools
dnl  **************************************************************************


dnl  **************************************************************************
dnl  NOTE:  This is only a selection of macros from the **Not Autotools**
dnl         project without documentation. For the entire collection and the
dnl         documentation please refer to the project's website.
dnl  **************************************************************************



dnl  n4_case_in(text, list1, if-found1[, ... listN, if-foundN], [if-not-found])
dnl  **************************************************************************
dnl
dnl  Searches for the first occurrence of `text` in each comma-separated list
dnl  `listN`
dnl
dnl  From: not-autotools/m4/not-m4sugar.m4
dnl
m4_define([n4_case_in],
	[m4_cond([m4_eval([$# < 2])], [1],
			[],
		[m4_argn([1], $2)], [$1],
			[$3],
		[m4_eval(m4_count($2)[ > 1])], [1],
			[n4_case_in([$1], m4_dquote(m4_shift($2)), m4_shift2($@))],
		[m4_eval([$# > 4])], [1],
			[n4_case_in([$1], m4_shift3($@))],
			[$4])])


dnl  n4_mem([macro-name1[, macro-name2[, ... macro-nameN]]], value)
dnl  **************************************************************************
dnl
dnl  Expands to `value` after this has been stored into one or more macros
dnl
dnl  From: not-autotools/m4/not-m4sugar.m4
dnl
m4_define([n4_mem],
	[m4_if([$#], [0], [],
		[$#], [1], [$1],
		[m4_define([$1], [$2])n4_mem(m4_shift($@))])])


dnl  NA_SANITIZE_VARNAME(string)
dnl  **************************************************************************
dnl
dnl  Replaces `/\W/g,` with `'_'` and `/^\d/` with `_\0`
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_SANITIZE_VARNAME],
	[m4_if(m4_bregexp(m4_normalize([$1]), [[0-9]]), [0], [_])[]m4_translit(m4_normalize([$1]),
		[ !"#$%&\'()*+,-./:;<=>?@[\\]^`{|}~],
		[__________________________________])])


dnl  NS_UNSET(var1[, var2[, var3[, ... varN]]])
dnl  **************************************************************************
dnl
dnl  Like `AS_UNSET()`, but it allows to unset many variables at once
dnl
dnl  From: not-autotools/m4/not-autoshell.m4
dnl
AC_DEFUN([NS_UNSET],
	[m4_ifnblank([$1], [AS_UNSET(m4_quote(m4_normalize([$1])));])m4_if([$#], [1], [], [NS_UNSET(m4_shift($@))])])


dnl  NS_FOR(initialization, statement)
dnl  **************************************************************************
dnl
dnl  M4 sugar to create a shell "for" loop
dnl
dnl  From: not-autotools/m4/not-autoshell.m4
dnl
AC_DEFUN([NS_FOR],
	[{ for $1; do[]m4_newline()$2[]m4_newline()done }])


dnl  NS_BREAK
dnl  **************************************************************************
dnl
dnl  M4 sugar that expands to a shell "break" command, to be used within loops
dnl
dnl  From: not-autotools/m4/not-autoshell.m4
dnl
AC_DEFUN([NS_BREAK],
	[m4_newline()break;m4_newline()])


dnl  NS_MOVEVAR(destination, source)
dnl  **************************************************************************
dnl
dnl  Copies the value of `source` into the shell variable `destination`, then
dnl  unsets `source` if this was set
dnl
dnl  From: not-autotools/m4/not-autoshell.m4
dnl
AC_DEFUN([NS_MOVEVAR],
	[{ AS_VAR_COPY([$1], [$2]); AS_UNSET([$2]); }])


dnl  NS_TEST_AEQ(string1, string2[, string3[, ... stringN]])
dnl  **************************************************************************
dnl
dnl  Check if all arguments expand to the same string and triggers `true` or
dnl  `false` accordingly
dnl
dnl  From: not-autotools/m4/not-autoshell.m4
dnl
AC_DEFUN([NS_TEST_AEQ],
	[m4_if([$#], [0], [:], [$#], [1], [:],
		[test "_AS_QUOTE(m4_dquote(?[]m4_joinall(?, m4_shift($@))))" = "_AS_QUOTE(m4_dquote(m4_for([], [2], [$#], [1], [[?$1]])))"])])


dnl  NS_TEST_NAE(string1, string2[, string3[, ... stringN]])
dnl  **************************************************************************
dnl
dnl  Check if all arguments do **not** expand to the same string and triggers
dnl  `true` or `false` accordingly
dnl
dnl  From: not-autotools/m4/not-autoshell.m4
dnl
AC_DEFUN([NS_TEST_NAE],
	[m4_if([$#], [0], [:], [$#], [1], [:],
		[test "_AS_QUOTE(m4_dquote(?[]m4_joinall(?, m4_shift($@))))" != "_AS_QUOTE(m4_dquote(m4_for([], [2], [$#], [1], [[?$1]])))"])])


dnl  NC_REQUIRE(macro1[, macro2[, macro3[, ... macroN]]])
dnl  **************************************************************************
dnl
dnl  Variadic version of `AC_REQUIRE()` that can be invoked also from the
dnl  global scope
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_REQUIRE],
	[m4_if([$#], [0],
		[m4_warn([syntax],
			[macro NC_REQUIRE() has been called without arguments])],
		[m4_ifblank([$1],
			[m4_warn([syntax],
				[ignoring empty argument in NC_REQUIRE()])],
			[AC_REQUIRE(m4_normalize([$1]))])[]m4_if([$#], [1], [],
			[m4_newline()NC_REQUIRE(m4_shift($@))])])])


dnl  NC_ARG_MISSING_WITHVAL(argument)
dnl  **************************************************************************
dnl
dnl  Checks whether `argument=*` has **not** been passed to the `configure`
dnl  script and launches `true` or `false` accordingly
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_ARG_MISSING_WITHVAL],
	[{ case " @S|@{@} " in *" _AS_QUOTE([$1])="*) false; ;; esac; }])


dnl  NC_MSG_WARNBOX(problem-description)
dnl  **************************************************************************
dnl
dnl  Wraps `problem-description` in a text box before passing it to
dnl  `AC_MSG_WARN()`
dnl
dnl  From: not-autotools/m4/not-ac-messages.m4
dnl
AC_DEFUN([NC_MSG_WARNBOX],
	[AC_MSG_WARN([m4_text_wrap([$1 --------------------------------------------------------------------],
		[         | ],
		[-----------------------------------------------------------],
		[79])])])


dnl  NC_CC_CHECK_POSIX([posix-version])
dnl  **************************************************************************
dnl
dnl  Checks whether the POSIX C API is available
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_CC_CHECK_POSIX], [
	AC_MSG_CHECKING([whether we have POSIX]m4_ifnblank([$1],
		[[ @{:@$1@:}@]]))
	AC_CACHE_VAL([ac_cv_have_posix]m4_ifnblank([$1], [[_$1]])[_c], [
		AC_EGREP_CPP([posix_supported], [
			@%:@define _POSIX_C_SOURCE ]m4_ifnblank([$1],
				[[$1]], [[200809L]])[
			@%:@include <unistd.h>
			@%:@ifdef _POSIX_VERSION
			]m4_ifnblank([$1],
				[[@%:@if _POSIX_VERSION == $1]])[
			posix_supported
			]m4_ifnblank([$1], [@%:@endif])[
			@%:@endif
		], [
			AS_VAR_SET([ac_cv_have_posix]m4_ifnblank([$1], [[_$1]])[_c],
				['yes'])
		], [
			AS_VAR_SET([ac_cv_have_posix]m4_ifnblank([$1], [[_$1]])[_c],
				['no'])
		])
	])
	AC_MSG_RESULT([${ac_cv_have_posix]m4_ifnblank([$1], [[_$1]])[_c}])
])


dnl  NC_CC_CHECK_SIZEOF(data-type[, headers[, store-as[, extra-sizes]]])
dnl  **************************************************************************
dnl
dnl  Checks for the size of `data-type` using **compile checks**, not run
dnl  checks.
dnl
dnl  From: not-autotools/m4/not-cc.m4
dnl
AC_DEFUN([NC_CC_CHECK_SIZEOF], [
	m4_pushdef([__label__],
		NA_SANITIZE_VARNAME([sizeof_]m4_tolower(m4_ifblank([$3],
			[[$1]], [[$3]]))))
	AC_MSG_CHECKING([size of `$1`])
	AC_CACHE_VAL([ac_cv_]__label__, [
		# List sizes in rough order of prevalence.
		for nc_sizeof in 4 8 1 2 16 m4_normalize([$4]) ; do
			AC_COMPILE_IFELSE([
				AC_LANG_PROGRAM([[$2]], [[
					switch (0) {
						case 0:
						case (sizeof ($1) ==
							${nc_sizeof}):;
					}
				]])
			],
				[AS_VAR_COPY([ac_cv_]__label__, [nc_sizeof])])
			AS_IF([test "x${ac_cv_]__label__[}" != x], [break;])
		done
	])
	AS_IF([test "x${ac_cv_]__label__[}" = x], [
		AC_MSG_RESULT([unknown])
		AC_MSG_ERROR([cannot determine a size for $1])
	])
	AC_MSG_RESULT([${ac_cv_]__label__[}])
	AC_DEFINE_UNQUOTED(m4_toupper(m4_quote(__label__)),
		[${ac_cv_]__label__[}],
		[The number of bytes in type $1])
	m4_ifnblank([$3],
		[AS_VAR_COPY([na_]m4_quote(m4_tolower([$3])), [nc_sizeof])])
	m4_popdef([__label__])
])


dnl  NC_CC_CHECK_CHAR_BIT
dnl  **************************************************************************
dnl
dnl  Calculates the size in bits of the `char` data type using compile checks
dnl
dnl  From: not-autotools/m4/not-cc.m4
dnl
AC_DEFUN([NC_CC_CHECK_CHAR_BIT], [
	AC_MSG_CHECKING([size of `char` in bits])
	AC_CACHE_VAL([ac_cv_char_bit], [
		# Minimum size in bits for `char` is guaranteed to be 8
		for nc_char_bit in {8..64}; do
			AC_COMPILE_IFELSE([
				AC_LANG_PROGRAM(, [[
					switch (0) {
						case 0: case ((unsigned char)
						(1 << ${nc_char_bit})):;
					}
				]])
			], [], [break])
		done
		AS_VAR_COPY([ac_cv_char_bit], [nc_char_bit])
	])
	AC_MSG_RESULT([${ac_cv_char_bit}])
	AC_DEFINE_UNQUOTED([COMPUTED_CHAR_BIT],
		[${ac_cv_char_bit}],
		[The number of bits in `char`])
])


dnl  NC_CONFIG_SHADOW_DIR(subdir)
dnl  **************************************************************************
dnl
dnl  Creates an extended configuration mode for files that rarely need to be
dnl  re-configured
dnl
dnl  Requires: `n4_case_in()`
dnl  From: not-autotools/m4/not-extended-config.m4
dnl
AC_DEFUN_ONCE([NC_CONFIG_SHADOW_DIR], [

	AC_REQUIRE([AC_PROG_LN_S])
	m4_define([NC_SHADOW_DIR], [$1])
	m4_define([NC_CONFNEW_SUBDIR], [confnew])
	m4_define([NC_THREATENED_LIST], [])
	AC_SUBST([confnewdir], [']NC_CONFNEW_SUBDIR['])

	AC_ARG_ENABLE([extended-config],
		[AS_HELP_STRING([--enable-extended-config@<:@=MODE@:>@],
			[extend the configure process to files that normally do not need
			to be re-configured, as their final content depends on upstream
			changes only and not on the state of this machine; possible values
			for MODE are: omitted or "yes" or "merge" for updating these files
			immediately, "sandbox" for safely putting their updated version
			into the `]m4_quote(NC_CONFNEW_SUBDIR)[` directory without modifying
			the package tree, or "no" for doing nothing @<:@default=no@:>@])],
			[AS_IF([test "x${enableval}" = x -o "x${enableval}" = xyes],
					[AS_VAR_SET([enable_extended_config], ['merge'])],
				[test "x${enableval}" != xsandbox -a "x${enableval}" != xmerge], [
					AS_VAR_SET([enable_extended_config], ['no'])
					AC_MSG_WARN([unrecognized option: --enable-extended-config='${enableval}'])
				])],
			[AS_VAR_SET([enable_extended_config], ['no'])])

	AM_CONDITIONAL([HAVE_EXTENDED_CONFIG], [test "x${enable_extended_config}" != xno])
	AM_CONDITIONAL([HAVE_UPDATES], [test "x${enable_extended_config}" = xsandbox])

	AM_COND_IF([HAVE_EXTENDED_CONFIG], [
		AS_MKDIR_P(["]m4_quote(NC_CONFNEW_SUBDIR)["])
		AC_CONFIG_COMMANDS([extended-config], [
			AS_IF([test "x${extconfmode}" = xmerge], [
				AS_VAR_SET([abs_srcdir], ["$(cd "${srcdir}" && pwd)"])
				echo "${threatlist}" | while read -r _FILE_; do
					mv "NC_CONFNEW_SUBDIR/${_FILE_}" "${srcdir}/${_FILE_}" && \
					(cd "$(dirname "NC_CONFNEW_SUBDIR/${_FILE_}")" && \
					${LN_S} "${abs_srcdir}/${_FILE_}" "$(basename "NC_CONFNEW_SUBDIR/${_FILE_}")")
				done
				AC_MSG_NOTICE([extended configuration has been merged with the package tree.])
				AS_UNSET([abs_srcdir])
			], [
				AC_MSG_NOTICE([extended configuration has been saved in ./NC_CONFNEW_SUBDIR.])
			])
			AS_UNSET([threatlist])
			AS_UNSET([extconfmode])
		], [
			AS_VAR_SET([extconfmode], ['${enable_extended_config}'])
			AS_VAR_SET([threatlist], ['${nc_threatlist}'])
		])
	])

	dnl  NC_THREATEN_FILES(file1[, file2[, file3[, ... fileN]]])
	dnl  **********************************************************************
	AC_DEFUN([NC_THREATEN_FILES], [
		AM_COND_IF([HAVE_EXTENDED_CONFIG], [
			AC_CONFIG_FILES(m4_foreach([_F_ITER_], m4_dquote(]m4_dquote(m4_map_args_sep([m4_normalize(], [)], [,], ][$][@][))[),
				[m4_ifnblank(m4_quote(_F_ITER_),
					[n4_case_in(m4_quote(_F_ITER_), m4_quote(NC_THREATENED_LIST),
						[n4_case_in(m4_quote(_F_ITER_), m4_quote(NC_SHADOW_REDEF), [],
							[m4_define([NC_SHADOW_REDEF],
								m4_ifset([NC_SHADOW_REDEF],
									[m4_dquote(NC_SHADOW_REDEF,[ ]_F_ITER_)],
									[m4_dquote(_F_ITER_)]))])],
						[m4_define([NC_THREATENED_LIST],
							m4_ifset([NC_THREATENED_LIST],
									[m4_dquote(NC_THREATENED_LIST, _F_ITER_)],
									[m4_dquote(_F_ITER_)]))
						m4_quote(NC_CONFNEW_SUBDIR[/]_F_ITER_[:]NC_SHADOW_DIR[/]_F_ITER_[.in])])])]))
		])
		AS_VAR_SET([nc_threatlist], ["]m4_join(m4_newline(), NC_THREATENED_LIST)["])
		m4_ifdef([NC_SHADOW_REDEF],
			[m4_warn([syntax], [redefined threatened files ]m4_quote(NC_SHADOW_REDEF)[ - skip])])
	])

	dnl  NC_THREATEN_BLINDLY
	dnl  **********************************************************************
	AC_DEFUN_ONCE([NC_THREATEN_BLINDLY],
		[NC_THREATEN_FILES(m4_shift(m4_bpatsubst(m4_quote(m4_esyscmd([find ']m4_quote(NC_SHADOW_DIR)[' -type f -name '*.in' -printf ", [[%P{/@/}]]"])), [\.in{/@/}], [])))])

	dnl  NC_SHADOW_AFTER_OUTPUT[(if-merge-cmds[, if-sandbox-cmds])]
	dnl  **********************************************************************
	AC_DEFUN_ONCE([NC_SHADOW_AFTER_OUTPUT],
		[m4_ifset([NC_THREATENED_LIST],
			[m4_ifnblank(m4_quote(]m4_dquote(][$][1][$][2][)[),
				[AM_COND_IF([HAVE_UPDATES],
					m4_ifblank(m4_quote(]m4_dquote(]m4_dquote(][$][2][)[)[),
						[[:]],
						[[m4_expand(m4_argn([2],
							]m4_dquote(]m4_dquote(]m4_dquote(]m4_dquote(][$][@][)[)[)[)[))]])[]m4_ifnblank(m4_quote(]m4_dquote(]m4_dquote(][$][1][)[)[), [,
							[AM_COND_IF([HAVE_EXTENDED_CONFIG],
								[m4_expand(m4_argn([1],
									]m4_dquote(]m4_dquote(]m4_dquote(]m4_dquote(]m4_dquote(][$][@][)[)[)[)[)[))])]]))])],
			[m4_warn([syntax], [NC_CONFIG_SHADOW_DIR has been invoked but no files have been threatened.])])])

])


dnl  NC_REQ_PROGS(prog1[, descr1][, prog2[, descr2][, ... progN[, descrN]]])
dnl  **************************************************************************
dnl
dnl  Checks whether one or more programs have been provided by the user or can
dnl  be retrieved automatically, generating an error if both conditions are
dnl  absent
dnl
dnl  Requires: `NA_SANITIZE_VARNAME()`
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_REQ_PROGS],
	[m4_ifnblank([$1],
		[m4_pushdef([_lit_], m4_quote(m4_toupper(NA_SANITIZE_VARNAME([$1]))))
		AC_ARG_VAR(_lit_,
			m4_default_quoted(m4_normalize([$2]), [$1 utility]))
		AS_IF([test "x@S|@{]_lit_[}" = x], [
			AC_PATH_PROG(_lit_, [$1])
			AS_IF([test "x@S|@{]_lit_[}" = x],
				[AC_MSG_ERROR([$1 utility not found])])[]m4_popdef([_lit_])
		])[]NC_REQ_PROGS(m4_shift2($@))])])


dnl  NA_HELP_STRINGS(list1, help1[, list2, help2[, ... listN, helpN]])
dnl  **************************************************************************
dnl
dnl  Similar to `AS_HELP_STRING()`, but with support for multiple strings, each
dnl  one associated with one or more options
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_HELP_STRINGS],
	[m4_if(m4_count($1), [1],
		[m4_if([$#], [0], [], [$#], [1],
			[m4_text_wrap($1, [  ])],
			[AS_HELP_STRING(m4_normalize($1), [$2])m4_if([$#], [2], [], [m4_newline()NA_HELP_STRINGS(m4_shift2($@))])])],
		[m4_text_wrap(m4_car($1)[,], [  ])m4_newline()NA_HELP_STRINGS(m4_dquote(m4_shift($1))m4_if([$#], [1], [], [, m4_shift($@)]))])])


dnl  NA_ESC_APOS(string)
dnl  **************************************************************************
dnl
dnl  Escapes all the occurrences of the apostrophe character in `string`
dnl
dnl  Requires: nothing
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_ESC_APOS],
	[m4_bpatsubst([$@], ['], ['\\''])])


dnl  NA_DOUBLE_DOLLAR(string)
dnl  **************************************************************************
dnl
dnl  Replaces all the occurrences of the dollar character in `string` with two
dnl  dollar characters (Makefile escaping)
dnl
dnl  Requires: nothing
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_DOUBLE_DOLLAR],
	[m4_bpatsubst([$@], [\$\|@S|@], [@S|@@S|@])])


dnl  NA_TRIANGLE_BRACKETS_TO_MAKE_VARS(string)
dnl  **************************************************************************
dnl
dnl  Replaces all variables enclosed within triangle brackets with Makefile
dnl  syntax for variables
dnl
dnl  Requires: nothing
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_TRIANGLE_BRACKETS_TO_MAKE_VARS],
	[m4_bpatsubst([$*], [<\([A-Za-z0-9_@*%<?^+|]+\)>],
		[m4_if(m4_len([\1]), [1],
			[@S|@\1],
			[@S|@@{:@\1@:}@])])])


dnl  NA_AMENDMENTS_SED_EXPR([amendment1[, amendment2[, ... amendmentN]]])
dnl  **************************************************************************
dnl
dnl  Creates a `sed` expression using all the "exception[-replacement_file]"
dnl  pairs passed as arguments ("amendments")
dnl
dnl  Requires: nothing
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_AMENDMENTS_SED_EXPR],
	[m4_ifblank([$1],
		['/!\s*START_EXCEPTION\s*@{:@@<:@^@:}@@:>@*@:}@\s*!/{d};/!\s*END_EXCEPTION\s*@{:@@<:@^@:}@@:>@*@:}@\s*!/{d};/!\s*ENTRY_POINT\s*@{:@@<:@^@:}@@:>@*@:}@\s*!/{d};/!\s*START_OMISSION\s*!/,/!\s*END_OMISSION\s*!/{d}'],
		['m4_ifnblank(m4_normalize(m4_argn([2], $1)), [/!\s*END_EXCEPTION\s*@{:@m4_normalize(m4_argn([1], $1))@:}@\s*!/{r '"m4_normalize(m4_argn([2], $1))"$'\n};/!\s*ENTRY_POINT\s*@{:@m4_normalize(m4_argn([1], $1))@:}@\s*!/{r '"m4_normalize(m4_argn([2], $1))"@S|@'\n};])/!\s*START_EXCEPTION\s*@{:@m4_normalize(m4_argn([1], $1))@:}@\s*!/,/!\s*END_EXCEPTION\s*@{:@m4_normalize(m4_argn([1], $1))@:}@\s*!/{d};/!\s*START_EXCEPTION\s*@{:@m4_normalize(m4_argn([1], $1))@:}@\s*!/{d};'NA_AMENDMENTS_SED_EXPR(m4_shift($@))])])


dnl  NA_AMEND(output-file, amendable-file[, ... amendmentN])
dnl  **************************************************************************
dnl
dnl  Creates a new file, amending a model with the content of one or more files
dnl
dnl  Requires: `NA_AMENDMENTS_SED_EXPR()`
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_AMEND],
	[{ echo 'Creating $1...'; sed NA_AMENDMENTS_SED_EXPR(m4_shift2($@)) "$2" > "$1"; }])


dnl  NC_SUBST_NOTMAKE(var[, value])
dnl  **************************************************************************
dnl
dnl  Calls `AC_SUBST(var[, value])` immediately followed by
dnl  `AM_SUBST_NOTMAKE(var)`
dnl
dnl  Requires: nothing
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_SUBST_NOTMAKE], [
	AC_SUBST([$1][]m4_if([$#], [0], [], [$#], [1], [], [, [$2]]))
	AM_SUBST_NOTMAKE([$1])
])


dnl  NC_GLOBAL_LITERALS(name1, [val1][, name2, [val2][, ... nameN, [valN]]])
dnl  **************************************************************************
dnl
dnl  For each `nameN`-`valN` pair, creates a new argumentless macro named
dnl  `[GL_]nameN` (where the `GL_` prefix stands for "Global Literal") and a
dnl  new output substitution named `nameN`, both expanding to `valN` when
dnl  invoked
dnl
dnl  Requires: `NA_SANITIZE_VARNAME()` and `NA_ESC_APOS()`
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_GLOBAL_LITERALS],
	[m4_pushdef([_lit_], m4_quote(NA_SANITIZE_VARNAME([$1])))[]m4_define([GL_]_lit_,
		m4_normalize([$2]))
	AC_SUBST(_lit_, ['NA_ESC_APOS(m4_normalize([$2]))'])[]m4_popdef([_lit_])[]m4_if(m4_eval([$# > 2]), [1],
		[NC_GLOBAL_LITERALS(m4_shift2($@))])])



dnl  **************************************************************************
dnl     NOTE:  The `GL_` prefix (which stands for "Global Literal"), the `NA_`
dnl            prefix (which stands for "Not Autotools"), the `NC_` prefix
dnl            (which stands for "Not autoConf"), the `NM_` prefix (which
dnl            stands for "Not autoMake"), the `NS_` prefix (which stands for
dnl            "Not autoShell") and the `n4_` prefix (which stands for "Not
dnl            m4sugar") are used with the purpose of avoiding collisions with
dnl            the default Autotools prefixes `AC_`, `AM_`, `AS_`, `AX_`,
dnl            `LT_`.
dnl  **************************************************************************


dnl  EOF

