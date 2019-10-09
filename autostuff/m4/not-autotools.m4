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
dnl  Note:  This is only a selection of macros from the Not Autotools project
dnl         without documentation. For the entire collection and the
dnl         documentation please refer to the project's website.
dnl  **************************************************************************



dnl  n4_lambda(macro_body)
dnl  **************************************************************************
dnl
dnl  Creates an anonymous macro on the fly, able to be passed as a callback
dnl  argument
dnl
dnl  From: not-autotools/m4/not-m4sugar.m4
dnl
m4_define([n4_lambda],
	[m4_pushdef([n4_anon], [$1])[]m4_pushdef([n4_anon], [m4_popdef([n4_anon])[]$1[]m4_popdef([n4_anon])])[]n4_anon])


dnl  n4_case_in(text, list1, if-found1[, ... listN, if-foundN], [if-not-found])
dnl  **************************************************************************
dnl
dnl  Searches for the first occurence of `text` in each comma-separated list
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


dnl  n4_charcode(character)
dnl  **************************************************************************
dnl
dnl  Returns the numeric value of a single code unit
dnl
dnl  Requires: `n4_lambda()`
dnl  From: not-autotools/m4/not-utf8.m4
dnl
m4_define([n4_charcode],
	[m4_changecom()m4_changequote()m4_changequote(<:, :>)m4_if(m4_len(<:$*:>), 0, 0, m4_incr(m4_index(<:]m4_quote(n4_lambda([m4_if(m4_eval([$1 > 0]), [1], [$0(m4_decr([$1]))m4_changecom()m4_changequote(<:, :>)m4_format(<:%c:>, <:$*:>)<::>m4_changequote([, ])m4_changecom([#])])])(255))[:>, <:$*:>)))m4_changequote([, ])m4_changecom([#])])


dnl  n4_codeunit_at(string[, index])
dnl  **************************************************************************
dnl
dnl  Returns the numeric value of the code unit at index `index` in the string
dnl  `string`
dnl
dnl  Requires: `n4_charcode()`
dnl  From: not-autotools/m4/not-utf8.m4
dnl
m4_define([n4_codeunit_at],
	[m4_if(m4_index([[$1]], [[(]]), m4_default([$2], [0]),
			[40],
		m4_index([[$1]], [[)]]), m4_default([$2], [0]),
			[41],
			[n4_charcode(m4_quote(m4_substr([$1], m4_default([$2], [0]), [1])))])])


dnl  n4_codepoint_to_ascii(unsigned-number[, escape-format-when-not-ascii])
dnl  **************************************************************************
dnl
dnl  Expands to the ASCII character whose codepoint is `unsigned-number` if the
dnl  latter is a 7-bit number, or to the string `\x[hex]` (where `[hex]` is the
dnl  hexadecimal representation of `unsigned-number`) if this is larger than
dnl  seven bits
dnl
dnl  From: not-autotools/m4/not-utf8.m4
dnl
m4_define([n4_codepoint_to_ascii],
	[m4_format(m4_if(m4_eval([$1 < 128]), [1], [%c], m4_default_quoted([$2], [\x%x])), [$1])])


dnl  n4_escape_non_ascii(string[, escape-format])
dnl  **************************************************************************
dnl
dnl  Replaces non-ASCII code points in `string` with their escaped hexadecimal
dnl  representation
dnl
dnl  Requires: `n4_codeunit_at()` and `n4_codepoint_to_ascii()`
dnl  From: not-autotools/m4/not-utf8.m4
dnl
m4_define([n4_escape_non_ascii],
	[m4_if(m4_eval(m4_len([$1])[ > 0]), [1],
		[n4_codepoint_to_ascii(n4_codeunit_at([$1], 0), [$2])])[]m4_if(m4_eval(m4_len([$1])[ > 1]), [1],
			[n4_escape_non_ascii(m4_quote(m4_substr([$1], 1)), [$2])])])


dnl  NA_SANITIZE_VARNAME(string)
dnl  **************************************************************************
dnl
dnl  Replaces `/\W/g,` with `'_'` and `/^\d/` with `V\0`
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NA_SANITIZE_VARNAME],
	[m4_if(m4_bregexp(m4_normalize([$1]), [[0-9]]), [0], [V])[]m4_translit(m4_normalize([$1]),
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


dnl  NC_CONFIG_SHADOW_DIR(subdir)
dnl  **************************************************************************
dnl
dnl  Creates an extended configuration mode for files that rarely need to be
dnl  re-configured
dnl
dnl  Requires: `n4_lambda()` and `n4_case_in()`
dnl  From: not-autotools/m4/not-extended-config.m4
dnl
AC_DEFUN_ONCE([NC_CONFIG_SHADOW_DIR], [

	m4_define([NC_SHADOW_DIR], [$1])
	m4_define([NC_CONFNEW_DIR], [confnew])
	m4_define([NC_THREATENED_LIST], [])
	AC_SUBST([CONFNEW_DIR], ['$(srcdir)/]NC_CONFNEW_DIR['])

	AC_ARG_ENABLE([extended-config],
		[AS_HELP_STRING([--enable-extended-config@<:@=MODE@:>@],
			[extend the configure process to files that normally do not need
			to be re-configured, as their final content depends on upstream
			changes only and not on the state of this machine; possible values
			for MODE are: omitted or "merge" for updating these files
			immediately, "sandbox" for safely putting their updated version
			into the "]m4_quote(NC_CONFNEW_DIR)[" subdirectory without
			modifying the package tree, or "no" for doing nothing
			@<:@default=no@:>@])],
			[AS_IF([test "x${enableval}" = x -o "x${enableval}" = xyes],
					[AS_VAR_SET([enable_extended_config], ['merge'])],
				[test "x${enableval}" != xsandbox -a "x${enableval}" != xmerge], [
					AS_VAR_SET([enable_extended_config], ['no'])
					AC_MSG_WARN([unrecognized option: --enable-extended-config='${enableval}'])
				])],
			[AS_VAR_SET([enable_extended_config], ['no'])])

	AM_CONDITIONAL([HAVE_EXTENDED_CONFIG], [test "x${enable_extended_config}" != xno])
	AM_CONDITIONAL([HAVE_UPDATES], [test "x${enable_extended_config}" = xsandbox])
	AM_COND_IF([HAVE_EXTENDED_CONFIG], [AS_MKDIR_P(["${srcdir}/]m4_quote(NC_CONFNEW_DIR)["])])

	AC_DEFUN([NC_THREATEN_FILES], [
		AM_COND_IF([HAVE_EXTENDED_CONFIG], [
			AC_CONFIG_FILES(m4_foreach([_F_ITER_], m4_dquote(]m4_dquote(][$][@][)[),
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
					m4_quote([${srcdir}/]NC_CONFNEW_DIR[/]_F_ITER_[:]NC_SHADOW_DIR[/]_F_ITER_[.in])])]))
		])

		m4_ifdef([NC_SHADOW_REDEF],
			[m4_warn([syntax], [redefined threatened files ]m4_quote(NC_SHADOW_REDEF)[ - skip])])
	])

	AC_DEFUN_ONCE([NC_THREATEN_BLINDLY],
		[NC_THREATEN_FILES(m4_shift(m4_bpatsubst(m4_quote(m4_esyscmd([find ']m4_quote(NC_SHADOW_DIR)[' -type f -name '*.in' -printf ", [[%P{/@/}]]"])), [\.in{/@/}], [])))])

	AC_DEFUN_ONCE([NC_SHADOW_MAYBE_OUTPUT], [
		m4_ifset([NC_THREATENED_LIST], [
			AM_COND_IF([HAVE_UPDATES],
				[AC_MSG_NOTICE([extended configuration has been saved in ${srcdir}/]m4_quote(NC_CONFNEW_DIR)[.])],
				[AM_COND_IF([HAVE_EXTENDED_CONFIG], [
					cp -rf "${srcdir}/]m4_quote(NC_CONFNEW_DIR)["/* "${srcdir}"/ && \
						rm -rf "${srcdir}/]m4_quote(NC_CONFNEW_DIR)["
					AC_MSG_NOTICE([extended configuration has been merged with the package tree.])
				])])
		], [
			m4_warn([syntax], [NC_SHADOW_MAYBE_OUTPUT has been invoked but no files have been threatened.])
		])
	])

])


dnl  NC_REQ_PROGS(prog1, [descr1][, prog2, [descr2][, ... progN, [descrN]]])
dnl  **************************************************************************
dnl
dnl  Checks whether one or more programs have been provided by the user or can
dnl  be retrieved automatically
dnl
dnl  Requires: `NA_SANITIZE_VARNAME()`
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_REQ_PROGS], [
	AC_ARG_VAR(m4_toupper(NA_SANITIZE_VARNAME([$1])),
		m4_default_quoted(m4_normalize([$2]), [$1 utility]))
	AS_IF([test "x@S|@{]m4_toupper(NA_SANITIZE_VARNAME([$1]))[}" = x], [
		AC_PATH_PROG(m4_toupper(NA_SANITIZE_VARNAME([$1])), [$1])
		AS_IF([test "x@S|@{]m4_toupper(NA_SANITIZE_VARNAME([$1]))[}" = x],
			[AC_MSG_ERROR([$1 utility not found])])
	])
	m4_if(m4_eval([$# > 2]), [1], [NC_REQ_PROGS(m4_shift2($@))])
])


dnl  NA_HELP_STRINGS(list1, help1[, list2, help2[, ... listN, helpN]])
dnl  **************************************************************************
dnl
dnl  Similar to `AS_HELP_STRING()`, but with support for multiple strings, each
dnl  one associated with one or more options
dnl
dnl  From: not-autotools/m4/not-autotools.m4
dnl
m4_define([NA_HELP_STRINGS],
	[m4_if(m4_count($1), [1],
		[m4_if([$#], [0], [], [$#], [1],
			[m4_text_wrap($1, [  ])],
			[AS_HELP_STRING(m4_normalize($1), [$2])m4_if([$#], [2], [], [m4_newline()NA_HELP_STRINGS(m4_shift2($@))])])],
		[m4_text_wrap(m4_argn(1, $1)[,], [  ])m4_newline()NA_HELP_STRINGS(m4_dquote(m4_shift($1))m4_if([$#], [1], [], [, m4_shift($@)]))])])


dnl  NC_SET_GLOBALLY(name1, [value1][, name2, [value2][, ... nameN, [valueN]]])
dnl  **************************************************************************
dnl
dnl  For each `nameN`-`valueN` pair, creates a new argumentless macro named
dnl  `[GL_]nameN` (where the `GL_` prefix stands for "Global Literal") and a new
dnl  output substitution named `nameN`, both expanding to `valueN` when invoked
dnl
dnl  Requires: `NA_SANITIZE_VARNAME()`
dnl  From: not-autotools/m4/not-autotools.m4
dnl
AC_DEFUN([NC_SET_GLOBALLY], [
	m4_define([GL_]NA_SANITIZE_VARNAME([$1]), m4_normalize([$2]))
	AC_SUBST(NA_SANITIZE_VARNAME([$1]), ['m4_bpatsubst(m4_normalize([$2]), ['], ['\\''])'])
	m4_if(m4_eval([$# > 2]), [1], [NC_SET_GLOBALLY(m4_shift2($@))])
])



dnl  **************************************************************************
dnl     Note:  The `GL_` prefix (which stands for "Global Literal"), the `NA_`
dnl            prefix (which stands for "Not Autotools"), the `NC_` prefix
dnl            (which stands for "Not autoConf"), the `NM_` prefix (which
dnl            stands for "Not autoMake"), the `NS_` prefix (which stands for
dnl            "Not autoShell") and the `n4_` prefix (which stands for "Not
dnl            m4sugar") are used with the purpose of avoiding collisions with
dnl            the default Autotools prefixes `AC_`, `AM_`, `AS_`, `AX_`,
dnl            `LT_`.
dnl  **************************************************************************


dnl  EOF

