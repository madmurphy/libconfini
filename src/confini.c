/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */

/**

	@file		confini.c
	@brief		libconfini functions
	@author		Stefano Gioffr&eacute;
	@copyright	GNU General Public License, version 3 or any later version
	@version	1.16.0
	@date		2016-2020
	@see		https://madmurphy.github.io/libconfini

**/


           /*/|
          (_|_)      _ _ _                      __ _       _
                    | (_) |__   ___ ___  _ __  / _(_)_ __ (_)
                    | | | '_ \ / __/ _ \| '_ \| |_| | '_ \| |
                    | | | |_) | (_| (_) | | | |  _| | | | | |
                    |_|_|_.__/ \___\___/|_| |_|_| |_|_| |_|_|      _ _
                                                                  ( | )
                                                                  |/*/



/**


	@def		INIFORMAT_TABLE_AS(_____)

	Content of the table:

	- Bits 1-19: INI syntax
	- Bits 20-22: INI semantics
	- Bits 23-24: Human syntax (disabled entries)



	@typedef	int (* IniStatsHandler) (
					IniStatistics * statistics,
					void * user_data
				)

	@param		statistics
					A pointer to the #IniStatistics to handle
	@param		user_data
					The custom argument previously passed to the caller function



	@typedef	int (* IniDispHandler) (
					IniDispatch *dispatch,
					void * user_data
				)

	@param		dispatch
					A pointer to the #IniDispatch to handle
	@param		user_data
					The custom argument previously passed to the caller function



	@typedef	int (* IniStrHandler) (
					char * ini_string,
					size_t string_length,
					size_t string_num,
					IniFormat format,
					void * user_data
				)

	@param		ini_string
					The INI string to handle
	@param		string_length
					The length of the INI string in bytes
	@param		string_num
					The unique number that identifies @p ini_string within a
					sequence of INI strings; it equals zero if @p ini_string is the
					first or the only member of the sequence
	@param		format
					The format of the INI file from which @p ini_string has been
					extracted
	@param		user_data
					The custom argument previously passed to the caller function



	@typedef	int (* IniSubstrHandler) (
					const char * ini_string,
					size_t fragm_offset,
					size_t fragm_length,
					size_t fragm_num,
					IniFormat format,
					void * user_data
				)

	@param		ini_string
					The INI string containing the fragment to handle
	@param		fragm_offset
					The offset of the selected fragment in bytes
	@param		fragm_length
					The length of the selected fragment in bytes
	@param		fragm_num
					The unique number that identifies the selected fragment within a
					sequence of fragments of @p ini_string; it equals zero if the
					fragment is the first or the only member of the sequence
	@param		format
					The format of the INI file from which @p ini_string has been
					extracted
	@param		user_data
					The custom argument previously passed to the caller function



	@struct		IniFormat

	@version	1.0
	@date		October 6th, 2018 (version 1.7.0 of the library)

	@property	IniFormat::delimiter_symbol
					The key-value delimiter character (ASCII only allowed); if set
					to `\0`, any space is delimiter
					(`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`); if, within the format
					given, `IniFormat::delimiter_symbol` matches a metacharacter
					(`'\\'`, `'\''`, `'\"'`), its role as metacharacter will have
					higher priority than its role as delimiter symbol (i.e., the
					format will have no key-value delimiter); you may use the
					#IniDelimiters `enum` for this.
	@property	IniFormat::case_sensitive
					If set to `true`, string comparisons will be always
					case-sensitive.
	@property	IniFormat::semicolon_marker
					The role of the semicolon character (use `enum`
					#IniCommentMarker for this).
	@property	IniFormat::hash_marker
					The role of the hash character (use `enum` #IniCommentMarker for
					this).
	@property	IniFormat::section_paths
					Defines whether and how the format supports sections (use `enum`
					#IniSectionPaths for this).
	@property	IniFormat::multiline_nodes
					Defines which class of entries are allowed to be multi-line (use
					`enum` #IniMultiline for this).
	@property	IniFormat::no_spaces_in_names
					If set to `true`, key and section names containing spaces (even
					within quotes) will be dispatched as #INI_UNKNOWN. Note that
					setting #IniFormat::delimiter_symbol to #INI_ANY_SPACE will not
					automatically set this option to `true` (spaces will still be
					allowed in section names).
	@property	IniFormat::no_single_quotes
					If set to `true`, the single-quote character (`'`) will be
					considered as a normal character.
	@property	IniFormat::no_double_quotes
					If set to `true`, the double-quote character (`"`) will be
					considered as a normal character.
	@property	IniFormat::implicit_is_not_empty
					If set to `true`, implicit keys (see @ref libconfini) will
					be always dispatched using the values provided by the global
					variables #INI_GLOBAL_IMPLICIT_VALUE and
					#INI_GLOBAL_IMPLICIT_V_LEN for the fields #IniDispatch::value
					and to #IniDispatch::v_len respectively; if set to `false`,
					implicit keys will be considered to be empty keys.
	@property	IniFormat::do_not_collapse_values
					If set to `true`, sequences of one or more spaces in values
					(`/\s+/`) will be dispatched verbatim.
	@property	IniFormat::preserve_empty_quotes
					If set to `true`, and if single/double quotes are
					metacharacters, ensures that, within values, empty strings
					enclosed between quotes (`""` or `''`) will not be collapsed
					together with the spaces that surround them. This option is
					useful for values containing space-delimited arrays, in order to
					preserve their empty members -- as in, for instance:
					`coordinates = "" ""`. Note that, in section and key names,
					empty strings enclosed between quotes are _always_ collapsed
					together with their surrounding spaces.
	@property	IniFormat::disabled_after_space
					If set to `true`, what follows `/[#;]\s/` is allowed to be
					parsed as a disabled entry.
	@property	IniFormat::disabled_can_be_implicit
					If set to `false`, comments that do not contain a key-value
					delimiter will never be parsed as disabled keys, but always as
					simple comments (even if the format supports implicit keys).



	@struct		IniStatistics

	@property	IniStatistics::format
					The format of the INI file (see #IniFormat)
	@property	IniStatistics::bytes
					The size of the parsed file in bytes
	@property	IniStatistics::members
					The size of the parsed file in number of members (nodes) -- this
					number always equals the number of dispatches that will be
					produced by #load_ini_file(), #load_ini_path() or
					#strip_ini_cache()



	@struct		IniDispatch

	@property	IniDispatch::format
					The format of the INI file (see #IniFormat)
	@property	IniDispatch::type
					The dispatch type (see `enum` #IniNodeType)
	@property	IniDispatch::data
					It can contain a comment, a section path or a key name,
					depending on #IniDispatch::type; it cannot be `NULL`
	@property	IniDispatch::value
					It can contain the value of a key element, an empty string or it
					can point to the address pointed by the global variable
					#INI_GLOBAL_IMPLICIT_VALUE (_the latter is the only case in
					which `IniDispatch::value` can be `NULL`_)
	@property	IniDispatch::append_to
					The current section path; it cannot be `NULL`
	@property	IniDispatch::d_len
					The length of the string #IniDispatch::data
	@property	IniDispatch::v_len
					The length of the string #IniDispatch::value
	@property	IniDispatch::at_len
					The length of the string #IniDispatch::append_to
	@property	IniDispatch::dispatch_id
					The dispatch number (the first dispatch is number zero)



	@def		INI_DISABLED_FLAG

	Example #1:

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	if (dispatch->type & INI_DISABLED_FLAG) {
		printf("This is not a real INI node...\n");
	}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Example #2:

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	if ((dispatch->type | INI_DISABLED_FLAG) == INI_DISABLED_KEY) {
		printf("Hey! This is either an INI_KEY or an INI_DISABLED_KEY!\n");
	}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	See also #IniNodeType.


**/



		/*\
		|*|
		|*|     LOCAL ENVIRONMENT
		|*|    ________________________________
		\*/



		/*  PREPROCESSOR PREAMBLE  */


/*  String concatenation facilities  */
#define __PP_CAT__(STEM, ...) STEM ## __VA_ARGS__
#define __PP_UCAT__(STEM, ...) STEM ## _ ## __VA_ARGS__
#define __PP_EVALUCAT__(STEM, ...) __PP_UCAT__(STEM, __VA_ARGS__)

/*  Conditionals  */
#define __PP_IIF__(CONDITION, ...) \
	__PP_EVALUCAT__(__PP_UCAT__(__COND, CONDITION), _)(__VA_ARGS__, , )
#define __COND_0__(IF_TRUE, IF_FALSE, ...) IF_FALSE
#define __COND_1__(IF_TRUE, ...) IF_TRUE


		/*  PREPROCESSOR ENVIRONMENT  */


#ifndef CONFINI_IO_FLAVOR
/**

	@brief			The I/O API to use (possibly overridden via
					`-DCONFINI_IO_FLAVOR=[FLAVOR]`)

	Possible values are `CONFINI_STANDARD` and `CONFINI_POSIX`

**/
#define CONFINI_IO_FLAVOR CONFINI_STANDARD
#endif



		/*  PREPROCESSOR GLUE  */


#define _LIBCONFINI_PRIVATE_ITEM_(GROUP, ITEM) \
	__PP_EVALUCAT__(__PP_CAT__(_LIB, GROUP), __PP_CAT__(ITEM, _))
#define _LIBCONFINI_CURRENT_FLAVOR_GET_(NAME) \
	_LIBCONFINI_PRIVATE_ITEM_(CONFINI_IO_FLAVOR, NAME)
#define _LIBCONFINI_IS_FLAVOR_(NAME) \
	_LIBCONFINI_PRIVATE_ITEM_(CONFINI_IO_FLAVOR, FLAVOR) == \
		_LIBCONFINI_PRIVATE_ITEM_(NAME, FLAVOR)



		/*  AVAILABLE I/O FLAVORS  */


/*  `-DCONFINI_IO_FLAVOR=CONFINI_STANDARD` (C99 Standard, default)  */
#define _LIBCONFINI_STANDARD_SEOF_FN_(FILEPTR) fseek(FILEPTR, 0, SEEK_END)
#define _LIBCONFINI_STANDARD_FT_FN_(FILEPTR) ftell(FILEPTR)
#define _LIBCONFINI_STANDARD_FT_T_ long signed int
/*  Any unique non-zero integer to identify this I/O API  */
#define _LIBCONFINI_STANDARD_FLAVOR_ 1

/*  `-DCONFINI_IO_FLAVOR=CONFINI_POSIX`  */
#define _LIBCONFINI_POSIX_SEOF_FN_(FILEPTR) fseeko(FILEPTR, 0, SEEK_END)
#define _LIBCONFINI_POSIX_FT_FN_(FILEPTR) ftello(FILEPTR)
#define _LIBCONFINI_POSIX_FT_T_ off_t
/*  Any unique non-zero integer to identify this I/O API  */
#define _LIBCONFINI_POSIX_FLAVOR_ 2

/*  Define `_POSIX_C_SOURCE` macro when `CONFINI_IO_FLAVOR == CONFINI_POSIX`  */
#if _LIBCONFINI_IS_FLAVOR_(CONFINI_POSIX)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

/*  It is possible to add other I/O APIs here. Feel	free to contribute!  */



		/*  CHECKS  */


#if _LIBCONFINI_PRIVATE_ITEM_(CONFINI_IO_FLAVOR, FLAVOR) == 0
#error Unsupported I/O API defined in macro CONFINI_IO_FLAVOR
#endif



		/*  HEADERS  */

#include <stdlib.h>
#include "confini.h"



		/*  DEPRECATED FUNCTIONS AND VARIABLES  */


#ifdef ini_global_set_lowercase_mode
#undef ini_global_set_lowercase_mode
#endif

#ifdef INI_GLOBAL_LOWERCASE_MODE
#undef INI_GLOBAL_LOWERCASE_MODE
#endif



		/*  ALIASES  */


#define _LIBCONFINI_FALSE_ 0
#define _LIBCONFINI_TRUE_ 1
#define _LIBCONFINI_CHARBOOL_ unsigned char
#define _LIBCONFINI_SIMPLE_SPACE_ '\x20'
#define _LIBCONFINI_HT_ '\t'
#define _LIBCONFINI_FF_ '\f'
#define _LIBCONFINI_VT_ '\v'
#define _LIBCONFINI_CR_ '\r'
#define _LIBCONFINI_LF_ '\n'
#define _LIBCONFINI_BACKSLASH_ '\\'
#define _LIBCONFINI_OPEN_SECTION_ '['
#define _LIBCONFINI_CLOSE_SECTION_ ']'
#define _LIBCONFINI_SUBSECTION_ '.'
#define _LIBCONFINI_SEMICOLON_ ';'
#define _LIBCONFINI_HASH_ '#'
#define _LIBCONFINI_DOUBLE_QUOTES_ '"'
#define _LIBCONFINI_SINGLE_QUOTES_ '\''
#define _LIBCONFINI_SEEK_EOF_(FILEPTR) \
	_LIBCONFINI_CURRENT_FLAVOR_GET_(SEOF_FN)(FILEPTR)
#define _LIBCONFINI_FTELL_(FILEPTR) \
	_LIBCONFINI_CURRENT_FLAVOR_GET_(FT_FN)(FILEPTR)
#define _LIBCONFINI_OFF_T_ \
	_LIBCONFINI_CURRENT_FLAVOR_GET_(FT_T)



/*  The character that will replace sequences of one or more spaces (`/\s+/`)  */
#define _LIBCONFINI_COLLAPSED_ _LIBCONFINI_SIMPLE_SPACE_


/*

	These may be any characters in theory... But after left-trimming each node
	leading spaces work pretty well as markers...

*/
/*  Internal marker of standard comments  */
#define _LIBCONFINI_SC_INT_MARKER_ _LIBCONFINI_SIMPLE_SPACE_
/*  Internal marker of inline comments  */
#define _LIBCONFINI_IC_INT_MARKER_ _LIBCONFINI_HT_



		/*  FUNCTIONAL MACROS AND CONSTANTS  */


/*

	Check whether a character can be escaped within a given format

*/
#define _LIBCONFINI_IS_ESC_CHAR_(CHR, FMT) ( \
		CHR == _LIBCONFINI_BACKSLASH_ ? \
			!INIFORMAT_HAS_NO_ESC(FMT) \
		: CHR == _LIBCONFINI_DOUBLE_QUOTES_ ? \
			!FMT.no_double_quotes \
		: \
			CHR == _LIBCONFINI_SINGLE_QUOTES_ && !FMT.no_single_quotes \
	)


/*

	Check whether a character represents any marker within a given format

*/
#define _LIBCONFINI_IS_ANY_MARKER_(CHR, FMT) ( \
		CHR == _LIBCONFINI_HASH_ ? \
			FMT.hash_marker != INI_IS_NOT_A_MARKER \
		: \
			CHR == _LIBCONFINI_SEMICOLON_ && \
			FMT.semicolon_marker != INI_IS_NOT_A_MARKER \
	)


/*

	Check whether a character represents any marker except `INI_IGNORE` within a
	given format

*/
#define _LIBCONFINI_IS_COM_MARKER_(CHR, FMT) ( \
		CHR == _LIBCONFINI_HASH_ ? \
			FMT.hash_marker != INI_IS_NOT_A_MARKER && \
			FMT.hash_marker != INI_IGNORE \
		: \
			CHR == _LIBCONFINI_SEMICOLON_ && \
			FMT.semicolon_marker != INI_IS_NOT_A_MARKER && \
			FMT.semicolon_marker != INI_IGNORE \
	)


/*

	Check whether a character represents a marker of type `INI_DISABLED_OR_COMMENT`
	within a given format

*/
#define _LIBCONFINI_IS_DIS_MARKER_(CHR, FMT) ( \
		CHR == _LIBCONFINI_HASH_ ? \
			FMT.hash_marker == INI_DISABLED_OR_COMMENT \
		: \
			CHR == _LIBCONFINI_SEMICOLON_ && \
			FMT.semicolon_marker == INI_DISABLED_OR_COMMENT \
	)


/*

	Check whether a character represents a marker of type `INI_IGNORE` within a
	given format

*/
#define _LIBCONFINI_IS_IGN_MARKER_(CHR, FMT) ( \
		CHR == _LIBCONFINI_HASH_ ? \
			FMT.hash_marker == INI_IGNORE \
		: \
			CHR == _LIBCONFINI_SEMICOLON_ && \
			FMT.semicolon_marker == INI_IGNORE \
	)


/*

	Maybe in the future there will be support for UTF-8 case folding (although
	probably not -- see "Code considerations" in the Manual), but for now only
	ASCII...

*/
#define _LIBCONFINI_CHR_CASEFOLD_(CHR) (CHR > 0x40 && CHR < 0x5b ? CHR | 0x60 : CHR)


/*

	Constants related to `_LIBCONFINI_SPACES_`

*/
#define _LIBCONFINI_EOL_IDX_ 0
#define _LIBCONFINI_SPALEN_ 6


/*

	The list of space characters -- do not change its order or its content!

*/
static const char _LIBCONFINI_SPACES_[_LIBCONFINI_SPALEN_] = {
	_LIBCONFINI_LF_,
	_LIBCONFINI_CR_,
	_LIBCONFINI_VT_,
	_LIBCONFINI_FF_,
	_LIBCONFINI_HT_,
	_LIBCONFINI_SIMPLE_SPACE_
};


/*

	Possible depths of `_LIBCONFINI_SPACES_` (see function #is_some_space()).

	Please, consider the following three constants as belonging together to a
	virtual opaque `enum`.

*/
#define _LIBCONFINI_WITH_EOL_ -1
#define _LIBCONFINI_NO_EOL_ 1
#define _LIBCONFINI_JUST_S_T_ 3


/**

	@brief			A list of possible string representations of boolean pairs

	There may be infinite pairs here. Each pair must be organized according to the
	following order:

	1. Signifier of `false`
	2. Signifier of `true`

	@note	Everything **must** be lowercase in this list.

**/
static const char * const INI_BOOLEANS[][2] = {
	{ "no", "yes" },
	{ "false", "true" },
	{ "off", "on" }
};


/*

	Other constants related to `INI_BOOLEANS`

*/
#define _LIBCONFINI_BOOLLEN_ (sizeof(INI_BOOLEANS) / sizeof(const char * const [2]))



		/*  ABSTRACT UTILITIES  */


/**

	@brief			Check whether a character is a space
	@param			chr				The target character
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			A boolean: `true` if the character matches, `false` otherwise

**/
static inline _LIBCONFINI_CHARBOOL_ is_some_space (const char chr, const int_least8_t depth) {
	register int_least8_t idx = depth;
	while (++idx < _LIBCONFINI_SPALEN_ && chr != _LIBCONFINI_SPACES_[idx]);
	return idx < _LIBCONFINI_SPALEN_;
}


/**

	@brief			Soft left trim -- does not change the buffer
	@param			str				The target string
	@param			offs			The offset where to start the left trim
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The offset of the first non-space character

**/
static inline size_t ltrim_s (const char * const str, const size_t offs, const int_least8_t depth) {
	register size_t idx = offs;
	while (is_some_space(str[idx++], depth));
	return idx - 1;
}


/**

	@brief			Hard left trim -- **does** change the buffer
	@param			str				The target string
	@param			offs			The offset where to start the left trim
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The offset of the first non-space character

**/
static inline size_t ltrim_h (char * const str, const size_t offs, const int_least8_t depth) {
	register size_t idx = offs;
	while (is_some_space(str[idx], depth)) { str[idx++] = '\0'; }
	return idx;
}


/**

	@brief			Shifting left trim -- **does** change the buffer
	@param			str				The target string
	@param			offs			The offset where to start the left trim
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The new length of the string

**/
static inline size_t ltrim_hh (char * const str, const size_t offs, const int_least8_t depth) {
	register size_t idx_d = offs, idx_s = offs;
	while (is_some_space(str[idx_s++], depth));
	if (--idx_s - idx_d) {
		while ((str[idx_d++] = str[idx_s++]));
		for (idx_s = idx_d; str[idx_s]; str[idx_s++] = '\0');
		return idx_d - 1;
	}
	while (str[idx_s++]);
	return idx_s - 1;
}


/**

	@brief			Soft right trim -- does not change the buffer
	@param			str				The target string
	@param			len				The length of the string
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The length of the string until the last non-space character

**/
static inline size_t rtrim_s (const char * const str, const size_t len, const int_least8_t depth) {
	register size_t idx = len + 1;
	while (--idx > 0 && is_some_space(str[idx - 1], depth));
	return idx;
}


/**

	@brief			Hard right trim -- **does** change the buffer
	@param			str				The target string
	@param			len				The length of the string
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The new length of the string

**/
static inline size_t rtrim_h (char * const str, const size_t len, const int_least8_t depth) {
	register size_t idx = len;
	while (idx > 0 && is_some_space(str[idx - 1], depth)) { str[--idx] = '\0'; }
	return idx;
}


/**

	@brief			Unparsed soft right trim (right trim of `/(?:\s|\\[\n\r])+$/`)
					-- does not change the buffer
	@param			str				The target string
	@param			len				The length of the string
	@return			The length of the string until the last non-space character

**/
static inline size_t urtrim_s (const char * const str, const size_t len) {

	register uint_least8_t abcd = 1;
	register size_t idx = len;


	/* \                                /\
	\ */     continue_urtrim:          /* \
	 \/     ______________________     \ */


	if (idx < 1) {

		return idx;

	}

	switch (str[--idx]) {

		case _LIBCONFINI_VT_:
		case _LIBCONFINI_FF_:
		case _LIBCONFINI_HT_:
		case _LIBCONFINI_SIMPLE_SPACE_:

			abcd = 1;
			goto continue_urtrim;

		case _LIBCONFINI_LF_:
		case _LIBCONFINI_CR_:

			abcd = 3;
			goto continue_urtrim;

		case _LIBCONFINI_BACKSLASH_:

			if (abcd >>= 1) {

				goto continue_urtrim;

			}

	}

	return idx + 1;

}


/**

	@brief			Convert an ASCII string to lower case
	@param			str			The target string
	@return			Nothing

**/
static inline void string_tolower (char * const str) {
	for (register char * chrptr = str; *chrptr; chrptr++) {
		*chrptr = _LIBCONFINI_CHR_CASEFOLD_(*chrptr);
	}
}



		/*  CONCRETE UTILITIES  */


/**

	@brief			Unparsed hard left trim (left trim of
					`/^(?:\s+|\\[\n\r]|''|"")+/`) -- **does** change the buffer
	@param			srcstr			The target string (it may contain multi-line
									escape sequences)
	@param			offs			The offset where to start the left trim
	@param			format			The format of the INI file
	@return			The offset of the first non-trivial character

**/
static inline size_t qultrim_h (
	char * const srcstr,
	const size_t offs,
	const IniFormat format
) {

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		Erase the previous character
		FLAG_64		Erase this character
		FLAG_128	Continue the loop

	*/

	register uint_least8_t
		abcd = (format.no_double_quotes ? 130 : 128) | format.no_single_quotes;

	size_t idx = offs;

	do {

		abcd	=	!(abcd & 28) && is_some_space(srcstr[idx], _LIBCONFINI_NO_EOL_) ?
						(abcd & 207) | 64
					: !(abcd & 12) && (
							srcstr[idx] == _LIBCONFINI_LF_ ||
							srcstr[idx] == _LIBCONFINI_CR_
					) ?
						(
							abcd & 16 ?
								(abcd & 239) | 96
							:
								abcd | 64
						)
					: !(abcd & 25) && srcstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							abcd & 4 ?
								(abcd & 235) | 96
							:
								(abcd & 143) | 4
						)
					: !(abcd & 22) && srcstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							abcd & 8 ?
								(abcd & 231) | 96
							:
								(abcd & 159) | 8
						)
					: srcstr[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd & 159) ^ 16
					:
						abcd & 31;


		if (abcd & 32) {

			srcstr[idx - 1] = '\0';

		}

		if (abcd & 64) {

			srcstr[idx] = '\0';

		}

		idx++;

	} while (abcd & 128);

	return abcd & 28 ? idx - 2 : idx - 1;

}


/**

	@brief			Soft left trim within an unparsed disabled entry (left trim of
					`/(?:(?:^|\\?[\n\r])[ \t\v\f]*(?:#(?:[ \t\v\f]|''|"")*)?)+/`)
					-- does not change the buffer
	@param			srcstr			The target string (it may contain multi-line
									escape sequences)
	@param			offs			The offset where to start the left trim
	@param			format			The format of the INI file
	@return			The offset of the first non-trivial character

**/
static inline size_t dqultrim_s (
	const char * const srcstr,
	const size_t offs,
	const IniFormat format
) {

	/*

	Mask `abcd` (7 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		A new line has just begun
		FLAG_64		Continue the left trim

	*/


	register uint_least16_t abcd	=	format.no_single_quotes |
										(format.no_double_quotes << 1) |
										96;

	register size_t idx = offs;

	do {

		abcd	=	is_some_space(srcstr[idx], _LIBCONFINI_NO_EOL_) ?
						(
							abcd & 28 ?
								abcd & 63
							:
								abcd
						)
					: srcstr[idx] == _LIBCONFINI_LF_ || srcstr[idx] == _LIBCONFINI_CR_ ?
						(
							abcd & 12 ?
								(abcd & 47) | 32
							:
								(abcd & 111) | 32
						)
					: srcstr[idx] == _LIBCONFINI_BACKSLASH_ ?
						(
							abcd & 28 ?
								(abcd & 31) | 16
							:
								(abcd & 95) | 16
						)
					: (abcd & 32) && _LIBCONFINI_IS_ANY_MARKER_(srcstr[idx], format) ?
						abcd & 79
					: !(abcd & 54) && srcstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(abcd & 95) ^ 8
					: !(abcd & 57) && srcstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(abcd & 95) ^ 4
					: srcstr[idx] ?
						abcd & 31
					:
						abcd & 19;


		idx++;

	} while (abcd & 64);

	return abcd & 28 ? idx - 2 : idx - 1;

}


/**

	@brief			Get the position of the first occurrence out of quotes of a
					given character, stopping after a given number of charcters
	@param			str				The string where to search
	@param			chr				The character to to search
	@param			len				The maximum number of characters to read
	@param			format			The format of the INI file
	@return			The offset of the first occurrence of @p chr, or @p len if
					@p chr has not been not found

**/
static inline size_t getn_metachar_pos (
	const char * const str,
	const char chr,
	const size_t len,
	const IniFormat format
) {

	size_t idx = 0;

	/*

	Mask `abcd` (5 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes

	*/

	for (

		register uint_least8_t
			abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

			idx < len && (
				(abcd & 12) || (
					chr ?
						str[idx] != chr
					:
						!is_some_space(str[idx], _LIBCONFINI_WITH_EOL_)
				)
			);

		abcd	=	str[idx] == _LIBCONFINI_BACKSLASH_ ? abcd ^ 16
					: !(abcd & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abcd ^ 8
					: !(abcd & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abcd ^ 4
					: abcd & 15,
		idx++

	);

	return idx;

}


/**

	@brief			Get the position of the first occurrence out of quotes of a
					given character
	@param			str				The string where to search
	@param			chr				The character to to search
	@param			format			The format of the INI file
	@return			The offset of the first occurrence of @p chr or the length of
					@p str if @p chr has not been not found

**/
static inline size_t get_metachar_pos (
	const char * const str,
	const char chr,
	const IniFormat format
) {

	size_t idx = 0;

	/*

	Mask `abcd` (5 bits used):

		--> As in #getn_metachar_pos()

	*/

	for (

		register uint_least8_t
			abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

			str[idx] && (
				(abcd & 12) || (
					chr ?
						str[idx] != chr
					:
						!is_some_space(str[idx], _LIBCONFINI_NO_EOL_)
				)
			);

		abcd	=	str[idx] == _LIBCONFINI_BACKSLASH_ ? abcd ^ 16
					: !(abcd & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abcd ^ 8
					: !(abcd & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abcd ^ 4
					: abcd & 15,
		idx++

	);

	return idx;

}


/**

	@brief			Replace `/\\(\n\r?|\r\n?)[\t \v\f]*[#;]/` or `/\\(\n\r?|\r\n?)/`
					with `"$1"`
	@param			srcstr			The target string (it may contain multi-line
									escape sequences)
	@param			len				Length of the string
	@param			is_disabled		The string represents a disabled entry
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t unescape_cr_lf (
	char * const srcstr,
	const size_t len,
	const _LIBCONFINI_CHARBOOL_ is_disabled,
	const IniFormat format
) {

	register size_t idx_s = 0, idx_d = 0;
	register _LIBCONFINI_CHARBOOL_ eol_i = _LIBCONFINI_EOL_IDX_;
	register _LIBCONFINI_CHARBOOL_ is_escaped = _LIBCONFINI_FALSE_;
	size_t probe;

	while (idx_s < len) {

		if (
			is_escaped && (
				srcstr[idx_s] == _LIBCONFINI_SPACES_[eol_i] ||
				srcstr[idx_s] == _LIBCONFINI_SPACES_[eol_i ^= 1]
			)
		) {

			srcstr[idx_d - 1] = srcstr[idx_s++];

			if (srcstr[idx_s] == _LIBCONFINI_SPACES_[eol_i ^ 1]) {

				srcstr[idx_d++] = srcstr[idx_s++];

			}

			if (is_disabled) {

				probe = ltrim_s(srcstr, idx_s, _LIBCONFINI_NO_EOL_);

				if (_LIBCONFINI_IS_DIS_MARKER_(srcstr[probe], format)) {

					idx_s = probe + 1;

				}

			}

			is_escaped = _LIBCONFINI_FALSE_;

		} else {

			is_escaped	=	srcstr[idx_s] == _LIBCONFINI_BACKSLASH_ ?
								!is_escaped
							:
								_LIBCONFINI_FALSE_;


			srcstr[idx_d++] = srcstr[idx_s++];

		}

	}

	for (idx_s = idx_d; idx_s < len; srcstr[idx_s++] = '\0');

	return idx_d;

}


/**

	@brief			Sanitize a section path
	@param			secpath			The section path
	@param			format			The format of the INI file
	@return			The new length of the string

	Out of quotes, similar to ECMAScript
	`secpath.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/\s+/g, " ")`

	A section path can start with a dot (append), but cannot end with a dot. Spaces
	surrounding dots will be removed. Fragments surrounded by single or double
	quotes (if these are enabled) are prevented from changes.

**/
static size_t sanitize_section_path (char * const secpath, const IniFormat format) {

	/*

	Mask `abcd` (12 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		These are initial spaces
		FLAG_64		This is a space out of quotes
		FLAG_128	This is a dot out of quotes
		FLAG_256	This is anything *but* an opening single/double quote
		FLAG_512	Don't ignore the last two characters
		FLAG_1024	Don't overwrite the previous character
		FLAG_2048	Path contains at least one name

	*/

	register uint_least16_t
		abcd = (format.no_double_quotes ? 1826 : 1824) | format.no_single_quotes;

	register size_t idx_s = 0, idx_d = 0;

	for (; secpath[idx_s]; idx_s++) {

		/*  Revision #2  */

		abcd	=	!(abcd & 12) && is_some_space(secpath[idx_s], _LIBCONFINI_WITH_EOL_) ?
						(
							abcd & 224 ?
								(abcd & 3055) | 832
							:
								(abcd & 4079) | 1856
						)
					: !(abcd & 12) && secpath[idx_s] == _LIBCONFINI_SUBSECTION_ ?
						(
							abcd & (abcd & 32 ? 128 : 192) ?
								(abcd & 2959) | 896
							:
								(abcd & 3983) | 1920
						)
					: !(abcd & 25) && secpath[idx_s] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							~abcd & 4 ?
								(abcd & 3839) | 1540
							: abcd & 256 ?
								(abcd & 3867) | 3840
							:
								(abcd & 3579) | 1280
						)
					: !(abcd & 22) && secpath[idx_s] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							~abcd & 8 ?
								(abcd & 3839) | 1544
							: abcd & 256 ?
								(abcd & 3863) | 3840
							:
								(abcd & 3575) | 1280
						)
					: secpath[idx_s] == _LIBCONFINI_BACKSLASH_ ?
						((abcd & 3871) | 3840) ^ 16
					:
						(abcd & 3855) | 3840;


		if (abcd & 512) {

			secpath[
				abcd & 1024 ?
					idx_d++
				: idx_d ?
					idx_d - 1
				:
					idx_d
			]					=	!(~abcd & 384) ?
										_LIBCONFINI_SUBSECTION_
									: !(~abcd & 320) ?
										_LIBCONFINI_COLLAPSED_
									:
										secpath[idx_s];

		} else if (idx_d) {

			idx_d--;

		}

	}

	for (

		idx_s	=	idx_d && (abcd & 2048) && (abcd & 192) ?
						--idx_d
					:
						idx_d;

			secpath[idx_s];

		secpath[idx_s++] = '\0'

	);

	return idx_d;

}


/**

	@brief			Out of quotes similar to ECMAScript
					`ini_string.replace(/''|""/g, "").replace(/^[\n\r]\s*|\s+/g, " ")`
	@param			ini_string		The string to collapse -- multi-line escape
									sequences must be already unescaped at
									this stage
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t collapse_everything (char * const ini_string, const IniFormat format) {

	/*

	Mask `abcd` (9 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		This is *not* a space out of quotes
		FLAG_64		This is an opening single/double quote
		FLAG_128	Don't ignore this character
		FLAG_256	Jump this character and the one before this

	*/

	register size_t idx_s = 0, idx_d = 0;

	register uint_least16_t
		abcd	=	(is_some_space(*ini_string, _LIBCONFINI_WITH_EOL_) ? 128 : 160) |
					(format.no_double_quotes << 1) |
					format.no_single_quotes;


	for (; ini_string[idx_s]; idx_s++) {

		/*  Revision #2  */

		abcd	=	!(abcd & 12) && is_some_space(ini_string[idx_s], _LIBCONFINI_WITH_EOL_) ?
						(
							abcd & 32 ?
								(abcd & 143) | 128
							:
								abcd & 47
						)
					: !(abcd & 25) && ini_string[idx_s] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							~abcd & 4 ?
								(abcd & 239) | 196
							: abcd & 64 ?
								(abcd & 299) | 256
							:
								(abcd & 171) | 160
						)
					: !(abcd & 22) && ini_string[idx_s] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							~abcd & 8 ?
								(abcd & 239) | 200
							: abcd & 64 ?
								(abcd & 295) | 256
							:
								(abcd & 167) | 160
						)
					: ini_string[idx_s] == _LIBCONFINI_BACKSLASH_ ?
						((abcd & 191) | 160) ^ 16
					:
						(abcd & 175) | 160;


		if (abcd & 256) {

			idx_d--;

		} else if (abcd & 128) {

			ini_string[idx_d++]		=	abcd & 44 ?
											ini_string[idx_s]
										:
											_LIBCONFINI_COLLAPSED_;

		}

	}

	for (

		idx_s	=	!(abcd & 32) && idx_d ?
						--idx_d
					:
						idx_d;

			ini_string[idx_s];

		ini_string[idx_s++] = '\0'

	);

	return idx_d;

}


/**

	@brief			Out of quotes similar to ECMAScript
					`ini_string.replace(/\s+/g, " ")`
	@param			ini_string		The string to collapse -- multi-line escape
									sequences must be already unescaped at this
									stage
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t collapse_spaces (char * const ini_string, const IniFormat format) {

	/*

	Mask `abcd` (7 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		This is a space out of quotes
		FLAG_64		Jump this character

	*/

	register uint_least8_t
		abcd = (format.no_double_quotes ? 34 : 32) | format.no_single_quotes;

	register size_t idx_s = 0;
	size_t idx_d = 0;

	for (; ini_string[idx_s]; idx_s++) {

		/*  Revision #1  */

		abcd	=	!(abcd & 12) && is_some_space(ini_string[idx_s], _LIBCONFINI_WITH_EOL_) ?
						(
							abcd & 32 ?
								(abcd & 111) | 64
							:
								(abcd & 47) | 32
						)
					: !(abcd & 25) && ini_string[idx_s] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(abcd & 15) ^ 4
					: !(abcd & 22) && ini_string[idx_s] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(abcd & 15) ^ 8
					: ini_string[idx_s] == _LIBCONFINI_BACKSLASH_ ?
						(abcd & 31) ^ 16
					:
						abcd & 15;


		if (~abcd & 64) {

			ini_string[idx_d++] = abcd & 32 ? _LIBCONFINI_COLLAPSED_ : ini_string[idx_s];

		}

	}

	for (

		idx_s	=	(abcd & 32) && idx_d ?
						--idx_d
					:
						idx_d;

			ini_string[idx_s];

		ini_string[idx_s++] = '\0'

	);

	return idx_d;

}


/**

	@brief			Similar to ECMAScript `str.replace(/''|""/g, "")`
	@param			str				The string to collapse
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t collapse_empty_quotes (char * const str, const IniFormat format) {

	/*

	Mask `abcd` (7 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		This is an opening single/double quote
		FLAG_64		These are empty quotes

	*/

	register uint_least8_t
		abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

	register size_t lshift = ltrim_s(str, 0, _LIBCONFINI_WITH_EOL_), idx = lshift;

	for (; str[idx]; idx++) {

		/*  Revision #1  */

		abcd	=	str[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd & 31) ^ 16
					: !(abcd & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							~abcd & 40 ?
								((abcd & 47) | 32) ^ 8
							:
								(abcd & 71) | 64
						)
					: !(abcd & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							~abcd & 36 ?
								((abcd & 47) | 32) ^ 4
							:
								(abcd & 75) | 64
						)
					:
						abcd & 15;


		str[idx - lshift] = str[idx];

		if (abcd & 64) {

			lshift += 2;

		}

	}

	for (idx -= lshift; str[idx]; str[idx++] = '\0');

	return rtrim_h(str, idx - lshift, _LIBCONFINI_WITH_EOL_);

}


/**

	@brief			Remove all comment initializers (`#` and/or `;`) from the
					beginning of each line of a comment
	@param			srcstr			The comment to parse (it may contain multi-line
									escape sequences)
	@param			len				The length of @p srcstr
	@param			format			The format of the INI file
	@return			The new length of the string

	- In multi-line comments: `srcstr.replace(/^[#;]+|(\n\r?|\r\n?)[\t \v\f]*[#;]+/g, "$1")`
	- In single-line comments: `srcstr.replace(/^[#;]+/, "")`

	The argument @p srcstr may begin with a comment initializer (`#` or `;`
	depending on the format), or with the character that immediately follows it.

**/
static size_t uncomment (char * const srcstr, size_t len, const IniFormat format) {

	register size_t idx_s = 0, idx_d = 0;

	if (format.multiline_nodes == INI_MULTILINE_EVERYWHERE) {

		/*  The comment can be multi-line  */

		/*

		Mask `abcd` (6 bits used):

			FLAG_1		Don't erase any character
			FLAG_2		We are in an odd sequence of backslashes
			FLAG_4		This new line character is escaped
			FLAG_8		This character is a comment character and follows
						`/(\n\s*|\r\s*)/`
			FLAG_16		This character is a part of a group of spaces that follow
						a new line (`/(\n|\r)[\t \v\f]+/`)
			FLAG_32		This character is *not* a new line character (`/[\r\n]/`)

		*/

		for (register uint_least8_t abcd = 8; idx_s < len; idx_s++) {

			abcd	=	srcstr[idx_s] == _LIBCONFINI_BACKSLASH_ ?
							((abcd & 35) | 32) ^ 2
						: srcstr[idx_s] == _LIBCONFINI_LF_ || srcstr[idx_s] == _LIBCONFINI_CR_ ?
							(abcd << 1) & 4
						: !(abcd & 32) && _LIBCONFINI_IS_ANY_MARKER_(srcstr[idx_s], format) ?
							(abcd & 40) | 8
						: !(abcd & 40) && is_some_space(srcstr[idx_s], _LIBCONFINI_NO_EOL_) ?
							(abcd & 57) | 16
						:
							(abcd & 33) | 32;


			if (!(abcd & 25)) {

				srcstr[abcd & 4 ? idx_d - 1 : idx_d++] = srcstr[idx_s];

			} else if (!(abcd & 28)) {

				idx_d++;

			}

		}

	} else {

		/*  The comment cannot be multi-line  */

		for (
			;
				idx_s < len &&
				_LIBCONFINI_IS_ANY_MARKER_(srcstr[idx_s], format);
			idx_s++
		);

		if (!idx_s) {

			return len;

		}

		for (; idx_s < len; srcstr[idx_d++] = srcstr[idx_s++]);

	}

	for (idx_s = idx_d; idx_s < len; srcstr[idx_s++] = '\0');

	return idx_d;

}


/**

	@brief			Try to determine the type of a member "as if it was active"
	@param			srcstr			String containing an individual node (it may
									contain multi-line escape sequences)
	@param			len				Length of the node
	@param			allow_implicit	A boolean: `true` if keys without a key-value
									delimiter are allowed, `false` otherwise
	@param			format			The format of the INI file
	@return			The node type (see header)

**/
static uint_least8_t get_type_as_active (
	const char * const srcstr,
	const size_t len,
	const _LIBCONFINI_CHARBOOL_ allow_implicit,
	const IniFormat format
) {

	const _LIBCONFINI_CHARBOOL_
		invalid_delimiter = _LIBCONFINI_IS_ESC_CHAR_(format.delimiter_symbol, format);

	if (
		!len || _LIBCONFINI_IS_ANY_MARKER_(*srcstr, format) || (
			*((unsigned char *) srcstr) == format.delimiter_symbol &&
			!invalid_delimiter
		)
	) {

		return INI_UNKNOWN;

	}

	register uint_least16_t abcd;
	register size_t idx;

	if (
		format.section_paths != INI_NO_SECTIONS &&
		*srcstr == _LIBCONFINI_OPEN_SECTION_
	) {

		if (format.no_spaces_in_names) {

			/*

				Search for the CLOSE SECTION character and possible spaces in names
				-- i.e., ECMAScript `/[^\.\s]\s+[^\.\s]/g.test(srcstr)`. The
				algorithm is made more complex by the fact that LF and CR characters
				are still escaped at this stage.

			*/

			/*

			Mask `abcd` (10 bits used):

				FLAG_1		Single quotes are not metacharacters (const)
				FLAG_2		Double quotes are not metacharacters (const)
				FLAG_4		Only one level of nesting is allowed (const)
				FLAG_8		Unescaped single quotes are odd right now
				FLAG_16		Unescaped double quotes are odd right now
				FLAG_32		We are in an odd sequence of backslashes
				FLAG_64		This is a space
				FLAG_128	What follows cannot contain spaces
				FLAG_256	Continue the loop
				FLAG_512	Section path is *not* valid

			*/


			idx = 1;

			abcd	=	(format.section_paths == INI_ONE_LEVEL_ONLY ? 772: 768) |
						(format.no_double_quotes << 1) |
						format.no_single_quotes;


			do {

				/*  Revision #2  */

				abcd	=	idx >= len ?
								abcd & 767
							: !(abcd & 42) && srcstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								(abcd & 991) ^ 16
							: !(abcd & 49) && srcstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								(abcd & 991) ^ 8
							: srcstr[idx] == _LIBCONFINI_LF_ || srcstr[idx] == _LIBCONFINI_CR_ ?
								(abcd & 991) | 64
							: is_some_space(srcstr[idx], _LIBCONFINI_NO_EOL_) ?
								(
									~abcd & 32 ?
										abcd | 64
									: ~abcd & 192 ?
										(abcd & 991) | 192
									:
										(abcd & 767) | 128
								)
							: !(abcd & 28) && srcstr[idx] == _LIBCONFINI_SUBSECTION_ ?
								(
									~abcd & 224 ?
										abcd & 799
									:
										abcd & 767
								)
							: !(abcd & 24) && srcstr[idx] == _LIBCONFINI_CLOSE_SECTION_ ?
								(
									~abcd & 224 ?
										abcd & 159
									:
										abcd & 767
								)
							: srcstr[idx] == _LIBCONFINI_BACKSLASH_ ?
								(
									~abcd & 32 ?
										abcd | 32
									: ~abcd & 192 ?
										(abcd & 991) | 128
									:
										(abcd & 735)
								)
							: ~abcd & 192 ?
								(abcd & 927) | 128
							:
								(abcd & 671) | 128;


				idx++;

			} while (abcd & 256);

			if (abcd & 512) {

				return INI_UNKNOWN;

			}

		} else if (
			(idx = getn_metachar_pos(
				srcstr,
				_LIBCONFINI_CLOSE_SECTION_,
				len,
				format
			) + 1) > len
		) {

			return INI_UNKNOWN;

		}

		/*

			Scan for possible non-space characters following the CLOSE SECTION
			character: if found the node cannot represent a section path (but it can
			possibly represent a key). Empty quotes will be tolerated.

		*/

		/*

		Recycling variable `abcd` (6 bits used)...:

			FLAG_1		Single quotes are not metacharacters (const)
			FLAG_2		Double quotes are not metacharacters (const)
			FLAG_4		Unescaped single quotes are odd right now
			FLAG_8		Unescaped double quotes are odd right now
			FLAG_16		We are in an odd sequence of backslashes
			FLAG_32		Continue the loop

		*/

		abcd = 32 | (format.no_double_quotes << 1) | format.no_single_quotes;

		do {

			if (idx >= len) {

				return INI_SECTION;

			}

			switch (srcstr[idx++]) {

				case _LIBCONFINI_VT_:
				case _LIBCONFINI_FF_:
				case _LIBCONFINI_HT_:
				case _LIBCONFINI_SIMPLE_SPACE_:

					abcd = abcd & 28 ? 0 : abcd & 47;
					continue;

				case _LIBCONFINI_LF_:
				case _LIBCONFINI_CR_:

					abcd = abcd & 12 ? 0 : abcd & 47;
					continue;

				case _LIBCONFINI_BACKSLASH_:

					abcd = abcd & 28 ? 0 : abcd | 16;
					continue;

				case _LIBCONFINI_DOUBLE_QUOTES_:

					abcd = abcd & 22 ? 0 : (abcd & 47) ^ 8;
					continue;

				case _LIBCONFINI_SINGLE_QUOTES_:

					abcd = abcd & 25 ? 0 : (abcd & 47) ^ 4;
					continue;

			}

			break;

		} while (abcd);

	}

	/*

		It can be just a key or `INI_UNKNOWN`...

	*/

	if (invalid_delimiter && !allow_implicit) {

		return INI_UNKNOWN;

	}

	/*

	Recycling variable `abcd` (2 bits used)...:

		FLAG_1		The delimiter **must** be present
		FLAG_2		Search for spaces in names

	*/

	abcd = (format.no_spaces_in_names << 1) | (allow_implicit ? 0 : 1);

	if (abcd) {

		idx = getn_metachar_pos(
			srcstr,
			(char) format.delimiter_symbol,
			len,
			format
		);

		if ((abcd & 1) && idx == len) {

			return INI_UNKNOWN;

		}

		if (abcd & 2) {

			idx = urtrim_s(srcstr, idx);

			do {

				if (is_some_space(srcstr[--idx], _LIBCONFINI_WITH_EOL_)) {

					return INI_UNKNOWN;

				}

			} while (idx);

		}

	}

	return INI_KEY;

}


/**

	@brief			Examine a (single-/multi-line) segment and check whether
					it contains more than just one node
	@param			srcstr			Segment to examine (it may contain multi-line
									escape sequences)
	@param			format			The format of the INI file
	@return			Number of entries found

**/
static size_t further_cuts (char * const srcstr, const IniFormat format) {

	/*

	Shared flags of mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Do not allow disabled entries after space (const)
		FLAG_8		Formats supports multi-line entries everywhere (const)
		FLAG_16		Formats supports multi-line entries everywhere except in
					comments (const)
		FLAG_32		Unescaped single quotes are odd right now
		FLAG_64		Unescaped double quotes are odd right now
		FLAG_128	We are in an odd sequence of backslashes

	*/

	register uint_least16_t
		abcd	=	((format.disabled_after_space << 2) ^ 4) |
					(format.no_double_quotes << 1) |
					format.no_single_quotes | (
						format.multiline_nodes == INI_MULTILINE_EVERYWHERE ?
							8
						: format.multiline_nodes == INI_BUT_COMMENTS ?
							16
						:
							0
					);


	register size_t idx;
	size_t focus_at, unparsable_at, search_at = 0, num_entries = 0;


	/* \                                /\
	\ */     search_for_cuts:          /* \
	 \/     ______________________     \ */


	if (!srcstr[search_at]) {

		return num_entries;

	}

	unparsable_at = 0;

	abcd	=	_LIBCONFINI_IS_DIS_MARKER_(srcstr[search_at], format) && (
					!(abcd & 4) ||
					!is_some_space(srcstr[search_at + 1], _LIBCONFINI_NO_EOL_)
				) ?
					(abcd & 31) | 2560
				: _LIBCONFINI_IS_IGN_MARKER_(srcstr[search_at], format) ?
					(abcd & 8 ? (abcd & 31) | 1024 : abcd & 31)
				: (abcd & 8) && (
					srcstr[search_at] == _LIBCONFINI_IC_INT_MARKER_ ||
					_LIBCONFINI_IS_ANY_MARKER_(srcstr[search_at], format)
				) ?
					(abcd & 31) | 3072
				:
					(abcd & 31) | 2048;


	if (abcd & 2048) {

		num_entries++;

	}

	if (abcd & 1536) {

		/*

			Node starts with `/[;#]/` and can be a disabled entry in any format, or
			a simple comment or a block that must be ignored in multi-line formats

		*/

		/*

		Mask `abcd` (14 bits used):

			FLAG_256	This or the previous character was not a space
			FLAG_512	We are in a disabled entry or a comment (semi-const)
			FLAG_1024	We are in a simple comment or in a block that must be ignored
						and format supports multi-line entries (semi-const)
			FLAG_2048	We are *not* in a block that must be ignored (semi-const)
			FLAG_4096	We have *not* just found an inline comment nested within a
						disabled entry
			FLAG_8192	We had previously found an inline comment nested in this
						segment, but the entry that preceded it had been checked and
						did not seem to represent a valid disabled entry

			NOTE:	As for FLAG_1-FLAG_16 I keep the values already assigned at the
					beginning of the function; all other flags are already set to
					zero. For the meaning of flags FLAG_1-FLAG_128 see the beginning
					of the function.

		*/

		idx = ltrim_s(srcstr, search_at + 1, _LIBCONFINI_NO_EOL_) - 1;


		/* \                                /\
		\ */     inactive_cut:             /* \
		 \/     ______________________     \ */


		switch (srcstr[++idx]) {

			case '\0':

				/*  End of string  */

				if (~abcd & 8) {

					/*

						Check if this is a valid disabled entry. If it is not,
						search for line breaks.

						If the code has reached this point it means that according
						to the format disabled entries can be multi-line but
						comments cannot, and #get_type_as_active() has never been
						invoked on this entry.

					*/

					focus_at = dqultrim_s(srcstr, search_at, format);

					if (
						srcstr[focus_at] && !get_type_as_active(
							srcstr + focus_at,
							idx - focus_at,
							format.disabled_can_be_implicit,
							format
						)
					) {

						srcstr[search_at] = _LIBCONFINI_SC_INT_MARKER_;
						unparsable_at = search_at + 1;

					}

				}

				break;

			case _LIBCONFINI_LF_:
			case _LIBCONFINI_CR_:

				/*

					Line break has been found in a multi-line disabled entry or
					a comment. Search for `/\\(?:\n\r?|\r\n?)\s*[^;#]/`.

				*/

				focus_at = dqultrim_s(srcstr, search_at, format);
				idx = ltrim_s(srcstr, idx + 1, _LIBCONFINI_WITH_EOL_);

				if (
					abcd & 2048 ?
						!(
							_LIBCONFINI_IS_DIS_MARKER_(srcstr[idx], format) &&
							(abcd & 24) && (
								(~abcd & 516) ||
								!is_some_space(srcstr[idx + 1], _LIBCONFINI_NO_EOL_)
							)
						) && !(
							_LIBCONFINI_IS_COM_MARKER_(srcstr[idx], format) &&
							(abcd & 8) && (
								((abcd ^ 512) & 8704) ||
								!get_type_as_active(
									srcstr + focus_at,
									idx - focus_at,
									format.disabled_can_be_implicit,
									format
								)
							)
						)
					:
						!_LIBCONFINI_IS_IGN_MARKER_(srcstr[idx], format)
				) {

					rtrim_h(srcstr, idx, _LIBCONFINI_WITH_EOL_);
					search_at = qultrim_h(srcstr, idx, format);
					goto search_for_cuts;

				}

				/*

					No case break here, keep it like this! `case /[ \t\v\f]/` must
					follow (switch case fallthrough).

				*/

			case _LIBCONFINI_VT_:
			case _LIBCONFINI_FF_:
			case _LIBCONFINI_HT_:
			case _LIBCONFINI_SIMPLE_SPACE_:

				abcd = (abcd & 15999) | 4096;
				goto inactive_cut;

			case _LIBCONFINI_BACKSLASH_:

				abcd = (abcd | 4352) ^ 128;
				goto inactive_cut;

			default:

				abcd	=	!(abcd & 1376) && (~abcd & 8200) &&
							_LIBCONFINI_IS_ANY_MARKER_(srcstr[idx], format) ?
								(abcd & 12159) | 256
							: !(abcd & 162) && srcstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abcd & 16255) | 4352) ^ 64
							: !(abcd & 193) && srcstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abcd & 16255) | 4352) ^ 32
							:
								(abcd & 16255) | 4352;


				if (abcd & 4096) {

					goto inactive_cut;

				}

				if (~abcd & 8192) {

					/*

						Inline comment has been found in a (supposedly) disabled
						entry.

					*/

					focus_at = dqultrim_s(srcstr, search_at, format);

					if (get_type_as_active(
						srcstr + focus_at,
						idx - focus_at,
						format.disabled_can_be_implicit,
						format
					)) {

						if (!_LIBCONFINI_IS_IGN_MARKER_(srcstr[idx], format)) {

							srcstr[idx] = _LIBCONFINI_IC_INT_MARKER_;
							num_entries++;

						}

						srcstr[idx - 1] = '\0';

						if (abcd & 8) {

							goto inactive_cut;

						}

						unparsable_at = idx + 1;

					} else {

						abcd |= 8192;
						srcstr[search_at] = _LIBCONFINI_SC_INT_MARKER_;

						if (abcd & 8) {

							goto inactive_cut;

						}

						unparsable_at = search_at + 1;

					}

				}

				/*  No case break here (last case)  */

		}

	} else if (_LIBCONFINI_IS_ANY_MARKER_(srcstr[search_at], format)) {

		/*

			Node starts with `/[;#]/` but cannot be multi-line or represent a
			disabled entry

		*/

		unparsable_at = search_at + 1;

	} else {

		/*

			Node is active: search for inline comments

		*/

		/*

		Recycling variable `abcd` (11 bits used)...:

			FLAG_256	Comment marker follows an escaped new line made of only one
						character (i.e., `"\\\n"` or `"\\\r"` but neither `"\\\r\n"`
						or `"\\\n\r"`)
			FLAG_512	This was neither a hash nor a semicolon character
			FLAG_1024	This was not a space

		NOTE:	As for FLAG_1-FLAG_16 I keep the values already assigned at the
				beginning of the function; all other flags are already set to zero
				(see previous usage of `abcd` within this function), with the only
				exception of FLAG_2048, which I am going to overwrite immediately.
				For the meaning of flags FLAG_1-FLAG_128 see the beginning of the
				function.

		*/

		abcd = (abcd & 2047) | 1536;
		idx = search_at;


		/* \                                /\
		\ */     active_cut:               /* \
		 \/     ______________________     \ */


		switch (srcstr[++idx]) {

			case '\0':

				/*  End of string  */
				break;

			case _LIBCONFINI_VT_:
			case _LIBCONFINI_FF_:
			case _LIBCONFINI_HT_:
			case _LIBCONFINI_SIMPLE_SPACE_:

				abcd = (abcd & 639) | 512;
				goto active_cut;

			case _LIBCONFINI_LF_:
			case _LIBCONFINI_CR_:

				abcd = (abcd & 639) | ((abcd << 1) & 256) | 512;
				goto active_cut;

			case _LIBCONFINI_BACKSLASH_:

				abcd = ((abcd & 1791) | 1536) ^ 128;
				goto active_cut;

			default:

				abcd	=	_LIBCONFINI_IS_ANY_MARKER_(srcstr[idx], format) ?
								abcd & 1407
							: !(abcd & 162) && srcstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abcd & 1791) | 1536) ^ 64
							: !(abcd & 193) && srcstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abcd & 1791) | 1536) ^ 32
							:
								(abcd & 1791) | 1536;

				if (abcd & 1760) {

					goto active_cut;

				}

				/*

					Inline comment has been found in an active entry.

				*/

				if (abcd & 256) {

					/*

						Remove the backslash if the comment immediately follows an
						escaped new line expressed by one chararacter
						(`/\\[\r\n]/`). In case of CR + LF or LF + CR
						(`/\\\n\r|\\\r\n/`) the backslash will be removed later by
						#strip_ini_cache().

					*/

					srcstr[idx - 2] = '\0';

				}

				srcstr[idx - 1] = '\0';

				if (!_LIBCONFINI_IS_IGN_MARKER_(srcstr[idx], format)) {

					srcstr[idx] = _LIBCONFINI_IC_INT_MARKER_;

					if (abcd & 8) {

						search_at = idx;
						goto search_for_cuts;

					}

					num_entries++;

				} else if (abcd & 8) {

					search_at = idx;
					goto search_for_cuts;

				}

				unparsable_at = idx + 1;
				/*  No case break here (last case)  */

		}

	}

	if (unparsable_at) {

		/*

			Cut unparsable multi-line comments

		*/

		for (idx = unparsable_at; srcstr[idx]; idx++) {

			if (srcstr[idx] == _LIBCONFINI_LF_ || srcstr[idx] == _LIBCONFINI_CR_) {

				search_at = qultrim_h(srcstr, idx, format);
				goto search_for_cuts;

			}

		}

	}

	return num_entries;

}

/** @startfnlist **/



		/*\
		|*|
		|*|     GLOBAL ENVIRONMENT
		|*|    ________________________________
		\*/



		/*  LIBRARY'S MAIN FUNCTIONS  */


												/** @utility{strip_ini_cache} **/
/**

	@brief			Parse and tokenize a buffer containing an INI file, then
					dispatch its content to a custom callback
	@param			ini_source		The buffer containing the INI file to tokenize
	@param			ini_length		The length of @p ini_source without counting the
									NUL terminator (if any -- se below)
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the
									first dispatch, or `NULL`
	@param			f_foreach		The function that will be invoked for each
									dispatch, or `NULL`
	@param			user_data		A custom argument, or `NULL`
	@return			Zero for success, otherwise an error code (see `enum`
					#ConfiniInterruptNo)

	The @p ini_source parameter must be a valid pointer to a buffer of size
	@p ini_length + 1 and cannot be `NULL`. The @p ini_source string does not need
	to be NUL-terminated, but _it does need one extra byte where to append a NUL
	terminator_ -- in fact, as soon as this function is invoked,
	`ini_source[ini_length]` will be immediately set to `\0`.

	In most cases, as when using `strlen()` for computing @p ini_length, this is not
	a concern, since `ini_source[ini_length]` will always be `\0` by the very
	definition of `strlen()`, and will only get overwritten with the same value.
	However, if you are passing a substring of a string, for example the fragment
	`foo=bar` of the string `foo=barracuda`, you must expect the string to be
	immediately truncated into `foo=bar\0acuda`.

	In other words, @p ini_source must point to a memory location where at least
	`ini_length + 1` bytes are freely usable.

	The user given function @p f_init (see #IniStatsHandler data type) will be
	invoked with two arguments: `statistics` (a pointer to an #IniStatistics
	structure containing some properties about the file read) and `user_data` (the
	custom argument @p user_data previously passed). If @p f_init returns a non-zero
	value the caller function will be interrupted.

	The user given function @p f_foreach (see #IniDispHandler data type) will be
	invoked with two arguments: `dispatch` (a pointer to an #IniDispatch structure
	containing the parsed member of the INI file) and `user_data` (the custom
	argument @p user_data previously passed). If @p f_foreach returns a non-zero
	value the caller function will be interrupted.

	After invoking `strip_ini_cache()`, the buffer pointed by the @p ini_source
	parameter must be considered as a _corrupted buffer_ and should be freed or
	overwritten. For more information about this function, please refer to the
	@ref libconfini.

	The parsing algorithms used by **libconfini** are able to parse any type of file
	encoded in 8-bit code units, as long as the characters that match the regular
	expression `/[\s\[\]\.\\;#"']/` refer to the same code points they refer to in
	ASCII (as they do, for example, in UTF-8 and ISO-8859-1), independently of
	platform-specific conventions.

	@note	In order to be null-byte-injection-safe, before dispatching the parsed
			content this function will strip all `NUL` characters possibly present
			in the buffer (with the exception of the last one).

	Possible return values are: #CONFINI_SUCCESS, #CONFINI_IINTR, #CONFINI_FEINTR,
	#CONFINI_EOOR.

	@include topics/strip_ini_cache.c

**/
int strip_ini_cache (
	register char * const ini_source,
	const size_t ini_length,
	const IniFormat format,
	const IniStatsHandler f_init,
	const IniDispHandler f_foreach,
	void * const user_data
) {

	const _LIBCONFINI_CHARBOOL_
		valid_delimiter = !_LIBCONFINI_IS_ESC_CHAR_(format.delimiter_symbol, format);

	_LIBCONFINI_CHARBOOL_ tmp_bool;
	register size_t idx, tmp_fast_size_t_1, tmp_fast_size_t_2;
	size_t tmp_size_t_1, tmp_size_t_2;

	ini_source[ini_length] = '\0';

	/*

		PART ONE: Examine and isolate each segment

	*/

	#define __ISNT_ESCAPED__ tmp_bool
	#define __LSHIFT__ tmp_fast_size_t_1
	#define __EOL_N__ tmp_fast_size_t_2
	#define __NL_AT__ tmp_size_t_1
	#define __N_MEMBERS__ tmp_size_t_2

	/*  UTF-8 BOM  */
	__LSHIFT__	=	*((unsigned char *) ini_source) == 0xEF &&
					*((unsigned char *) ini_source + 1) == 0xBB &&
					*((unsigned char *) ini_source + 2) == 0xBF
					? 3 : 0;

	for (

		__N_MEMBERS__ = 0,
		__EOL_N__ = _LIBCONFINI_EOL_IDX_,
		__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_,
		__NL_AT__ = 0,
		idx = __LSHIFT__;

			idx < ini_length;

		idx++

	) {

		ini_source[idx - __LSHIFT__] = ini_source[idx];

		if (
			ini_source[idx] == _LIBCONFINI_SPACES_[__EOL_N__] ||
			ini_source[idx] == _LIBCONFINI_SPACES_[__EOL_N__ ^= 1]
		) {

			if (format.multiline_nodes == INI_NO_MULTILINE || __ISNT_ESCAPED__) {

				ini_source[idx - __LSHIFT__] = '\0';
				__N_MEMBERS__ += further_cuts(
					ini_source + qultrim_h(ini_source, __NL_AT__, format),
					format
				);
				__NL_AT__ = idx - __LSHIFT__ + 1;

			} else if (ini_source[idx + 1] == _LIBCONFINI_SPACES_[__EOL_N__ ^ 1]) {

				idx++;
				ini_source[idx - __LSHIFT__] = ini_source[idx];

			}

			__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_;

		} else if (ini_source[idx] == _LIBCONFINI_BACKSLASH_) {

			__ISNT_ESCAPED__ = !__ISNT_ESCAPED__;

		} else if (ini_source[idx]) {

			__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_;

		} else {

			/*  Remove `NUL` characters from the buffer (if any)  */
			__LSHIFT__++;

		}

	}

	const size_t real_length = idx - __LSHIFT__;

	while (idx > real_length) {

		ini_source[--idx] = '\0';

	}

	__N_MEMBERS__ += further_cuts(
		ini_source + qultrim_h(ini_source, __NL_AT__, format),
		format
	);

	/*  Debug  */

	/*

	for (size_t tmp = 0; tmp < ini_length + 1; tmp++) {
		putchar(ini_source[tmp] == 0 ? '$' : ini_source[tmp]);
	}
	putchar('\n');

	*/

	IniStatistics this_doc = {
		.format = format,
		.bytes = ini_length,
		.members = __N_MEMBERS__
	};

	if (f_init && f_init(&this_doc, user_data)) {

		return CONFINI_IINTR;

	}

	#undef __N_MEMBERS__
	#undef __NL_AT__
	#undef __EOL_N__
	#undef __LSHIFT__
	#undef __ISNT_ESCAPED__

	/*

		PART TWO: Dispatch the parsed input

	*/

	if (!f_foreach) {

		return CONFINI_SUCCESS;

	}

	#define __NODE_AT__ tmp_fast_size_t_1
	#define __CURR_PARENT_LEN__ tmp_fast_size_t_2
	#define __SUBPARENT_LEN__ tmp_size_t_1
	#define __REAL_PARENT_LEN__ tmp_size_t_2
	#define __PARENT_IS_DISABLED__ tmp_bool

	__REAL_PARENT_LEN__ = __CURR_PARENT_LEN__ = __SUBPARENT_LEN__  = 0;

	char
		* curr_parent_str = ini_source + real_length,
		* subparent_str = curr_parent_str,
		* real_parent_str = curr_parent_str;

	IniDispatch dsp = {
		.format = format,
		.dispatch_id = 0
	};

	__PARENT_IS_DISABLED__ = _LIBCONFINI_FALSE_;

	for (__NODE_AT__ = 0, idx = 0; idx <= real_length; idx++) {

		if (ini_source[idx]) {

			continue;

		}

		if (
			!ini_source[__NODE_AT__] ||
			_LIBCONFINI_IS_IGN_MARKER_(ini_source[__NODE_AT__], format)
		) {

			__NODE_AT__ = idx + 1;
			continue;

		}

		if (dsp.dispatch_id >= this_doc.members) {

			return CONFINI_EOOR;

		}

		dsp.data = ini_source + __NODE_AT__;
		dsp.d_len = idx - __NODE_AT__;

		/*  We won't need `__NODE_AT__` for a while, so let's recycle it...  */
		#undef __NODE_AT__
		#define __ITER__ tmp_fast_size_t_1

		/*  Set `dsp.value` to an empty string  */
		dsp.value = ini_source + idx;
		dsp.v_len = 0;

		if (
			_LIBCONFINI_IS_DIS_MARKER_(*dsp.data, format) && (
				format.disabled_after_space ||
				!is_some_space(dsp.data[1], _LIBCONFINI_NO_EOL_)
			)
		) {

			__ITER__ = dqultrim_s(dsp.data, 0, format);

			dsp.type = get_type_as_active(
				dsp.data + __ITER__,
				dsp.d_len - __ITER__,
				format.disabled_can_be_implicit,
				format
			);

			if (dsp.type) {

				dsp.data += __ITER__;
				dsp.d_len -= __ITER__;

				/*

				// Not strictly needed...
				for (; __ITER__ > 0; dsp.data[--__ITER__] = '\0');

				*/

			}

			dsp.type |= INI_DISABLED_FLAG;

		} else {

			switch (*dsp.data) {

				default:

					if (!_LIBCONFINI_IS_ANY_MARKER_(*dsp.data, format)) {

						dsp.type = get_type_as_active(
							dsp.data,
							dsp.d_len,
							_LIBCONFINI_TRUE_,
							format
						);
						break;

					}

					/*

						No case break here, keep it like this!
						`case _LIBCONFINI_SC_INT_MARKER_` must follow
						(switch case fallthrough).

					*/

				case _LIBCONFINI_SC_INT_MARKER_:

					/*

					// Not strictly needed...
					*dsp.data = '\0';

					*/

					dsp.type = INI_COMMENT;

					break;

				case _LIBCONFINI_IC_INT_MARKER_:

					/*

					// Not strictly needed...
					*dsp.data = '\0';

					*/

					dsp.type = INI_INLINE_COMMENT;
					/*  No case break here (last case)  */

			}

		}

		if (__CURR_PARENT_LEN__ && __SUBPARENT_LEN__) {

			__ITER__ = 0;

			do {

				curr_parent_str[__CURR_PARENT_LEN__ + __ITER__] =
					subparent_str[__ITER__];

			} while (__ITER__++ < __SUBPARENT_LEN__);

			__CURR_PARENT_LEN__ += __SUBPARENT_LEN__;
			subparent_str = curr_parent_str + __CURR_PARENT_LEN__;
			__SUBPARENT_LEN__ = 0;

		}

		if (__PARENT_IS_DISABLED__ && !(dsp.type & INI_DISABLED_FLAG)) {

			real_parent_str[__REAL_PARENT_LEN__] = '\0';
			__CURR_PARENT_LEN__ = __REAL_PARENT_LEN__;
			curr_parent_str = real_parent_str;
			__PARENT_IS_DISABLED__ = _LIBCONFINI_FALSE_;

		} else if (!__PARENT_IS_DISABLED__ && dsp.type == INI_DISABLED_SECTION) {

			__REAL_PARENT_LEN__ = __CURR_PARENT_LEN__;
			real_parent_str = curr_parent_str;
			__PARENT_IS_DISABLED__ = _LIBCONFINI_TRUE_;

		}

		dsp.append_to = curr_parent_str;
		dsp.at_len = __CURR_PARENT_LEN__;

		if (dsp.type == INI_COMMENT || dsp.type == INI_INLINE_COMMENT) {

			dsp.d_len = uncomment(++dsp.data, dsp.d_len - 1, format);

		} else if (format.multiline_nodes != INI_NO_MULTILINE) {

			dsp.d_len = unescape_cr_lf(
				dsp.data,
				dsp.d_len,
				dsp.type & INI_DISABLED_FLAG,
				format
			);

		}

		switch (dsp.type) {

			/*

			case INI_UNKNOWN:

				// Do nothing

				break;

			*/

			case INI_SECTION:
			case INI_DISABLED_SECTION:

				*dsp.data++ = '\0';

				__ITER__ = getn_metachar_pos(
					dsp.data,
					_LIBCONFINI_CLOSE_SECTION_,
					dsp.d_len,
					format
				);

				while (dsp.data[__ITER__]) {

					dsp.data[__ITER__++] = '\0';

				}

				dsp.d_len	=	format.section_paths == INI_ONE_LEVEL_ONLY ?
									collapse_everything(dsp.data, format)
								:
									sanitize_section_path(dsp.data, format);


				if (
					format.section_paths == INI_ONE_LEVEL_ONLY ||
					*dsp.data != _LIBCONFINI_SUBSECTION_
				) {

					/*

						Append to root (this is an absolute path)

					*/

					curr_parent_str = dsp.data;
					__CURR_PARENT_LEN__ = dsp.d_len;
					subparent_str = ini_source + idx;
					__SUBPARENT_LEN__ = 0;
					dsp.append_to = subparent_str;
					dsp.at_len = 0;

				} else if (
					format.section_paths == INI_ABSOLUTE_ONLY ||
					!__CURR_PARENT_LEN__
				) {

					/*

						Append to root and remove the leading dot (parent is root or
						relative paths are not allowed)

					*/

					curr_parent_str = ++dsp.data;
					__CURR_PARENT_LEN__ = --dsp.d_len;
					subparent_str = ini_source + idx;
					__SUBPARENT_LEN__ = 0;
					dsp.append_to = subparent_str;
					dsp.at_len = 0;

				} else if (dsp.d_len != 1) {

					/*

						Append to the current parent (this is a relative path
						and parent is not root)

					*/

					subparent_str = dsp.data;
					__SUBPARENT_LEN__ = dsp.d_len;

				}

				if (INI_GLOBAL_LOWERCASE_MODE && !format.case_sensitive) {

					string_tolower(dsp.data);

				}

				break;

			case INI_KEY:
			case INI_DISABLED_KEY:

				if (
					valid_delimiter &&
					(__ITER__ = getn_metachar_pos(
						dsp.data,
						(char) dsp.format.delimiter_symbol,
						dsp.d_len,
						format
					)) < dsp.d_len
				) {

					dsp.data[__ITER__] = '\0';
					dsp.value = dsp.data + __ITER__ + 1;


					switch (
						(format.preserve_empty_quotes << 1) |
						format.do_not_collapse_values
					) {

						case 0:	dsp.v_len = collapse_everything(dsp.value, format); break;

						case 1:	dsp.v_len = collapse_empty_quotes(dsp.value, format); break;

						case 2:	dsp.v_len = collapse_spaces(dsp.value, format); break;

						case 4:

							dsp.value += ltrim_h(dsp.value, 0, _LIBCONFINI_WITH_EOL_);
							dsp.v_len = rtrim_h(
								dsp.value,
								dsp.d_len + dsp.data - dsp.value,
								_LIBCONFINI_WITH_EOL_
							);
							/*  No case break here (last case)  */

					}

				} else if (format.implicit_is_not_empty) {

					dsp.value = INI_GLOBAL_IMPLICIT_VALUE;
					dsp.v_len = INI_GLOBAL_IMPLICIT_V_LEN;

				}

				dsp.d_len = collapse_everything(dsp.data, format);

				if (INI_GLOBAL_LOWERCASE_MODE && !format.case_sensitive) {

					string_tolower(dsp.data);

				}

				break;

			case INI_COMMENT:
			case INI_INLINE_COMMENT:

				dsp.append_to = ini_source + idx;
				dsp.at_len = 0;
				/*  No case break here (last case)  */

		}

		if (f_foreach(&dsp, user_data)) {

			return CONFINI_FEINTR;

		}

		/*  Restore `__NODE_AT__` on **exactly** the same variable as before  */
		#undef __ITER__
		#define __NODE_AT__ tmp_fast_size_t_1
		dsp.dispatch_id++;
		__NODE_AT__ = idx + 1;

	}

	#undef __PARENT_IS_DISABLED__
	#undef __REAL_PARENT_LEN__
	#undef __SUBPARENT_LEN__
	#undef __CURR_PARENT_LEN__
	#undef __NODE_AT__

	return CONFINI_SUCCESS;

}


													/** @utility{load_ini_file} **/
/**

	@brief			Parse an INI file and dispatch its content to a custom callback
					using a `FILE` structure as argument
	@param			ini_file		The `FILE` handle pointing to the INI file to
									parse
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the
									first dispatch, or `NULL`
	@param			f_foreach		The function that will be invoked for each
									dispatch, or `NULL`
	@param			user_data		A custom argument, or `NULL`
	@return			Zero for success, otherwise an error code (see `enum`
					#ConfiniInterruptNo)

	@note	This function is absent if the `--without-io-api` option was passed to
			the `configure` script when the library was compiled

	The @p ini_file parameter must be a `FILE` handle with read privileges. On some
	platforms, such as Microsoft Windows, it might be needed to add the binary
	specifier to the mode string (`"b"`) in order to prevent discrepancies between
	the physical size of the file and its computed size:

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	FILE * my_file = fopen("example.conf", "rb");
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	For the two parameters @p f_init and @p f_foreach see function
	#strip_ini_cache().

	The parsing algorithms used by **libconfini** are able to parse any type of file
	encoded in 8-bit code units, as long as the characters that match the regular
	expression `/[\s\[\]\.\\;#"']/` refer to the same code points they refer to in
	ASCII (as they do, for example, in UTF-8 and ISO-8859-1), independently of
	platform-specific conventions.

	@note	In order to be null-byte-injection safe, `NUL` characters, if present in
			the file, will be removed from the dispatched strings.

	Possible return values are: #CONFINI_SUCCESS, #CONFINI_IINTR, #CONFINI_FEINTR,
	#CONFINI_ENOMEM, #CONFINI_EIO, #CONFINI_EOOR, #CONFINI_EBADF, #CONFINI_EFBIG.

	@include topics/load_ini_file.c

**/
int load_ini_file (
	FILE * const ini_file,
	const IniFormat format,
	const IniStatsHandler f_init,
	const IniDispHandler f_foreach,
	void * const user_data
) {

	_LIBCONFINI_OFF_T_ file_size;

	if (
		_LIBCONFINI_SEEK_EOF_(ini_file) ||
		(file_size = _LIBCONFINI_FTELL_(ini_file)) < 0
	) {

		return CONFINI_EBADF;

	}

	if (file_size > SIZE_MAX) {

		return CONFINI_EFBIG;

	}

	char * const cache = (char *) malloc((size_t) file_size + 1);

	if (!cache) {

		return CONFINI_ENOMEM;

	}

	rewind(ini_file);

	if (fread(cache, 1, (size_t) file_size, ini_file) < file_size) {

		free(cache);
		return CONFINI_EIO;

	}

	const int return_value = strip_ini_cache(
		cache,
		(size_t) file_size,
		format,
		f_init,
		f_foreach,
		user_data
	);

	free(cache);
	return return_value;

}


													/** @utility{load_ini_path} **/
/**

	@brief			Parse an INI file and dispatch its content to a custom callback
					using a path as argument
	@param			path			The path of the INI file
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the
									first dispatch, or `NULL`
	@param			f_foreach		The function that will be invoked for each
									dispatch, or `NULL`
	@param			user_data		A custom argument, or `NULL`
	@return			Zero for success, otherwise an error code (see `enum`
					#ConfiniInterruptNo)

	@note	This function is absent if the `--without-io-api` option was passed to
			the `configure` script when the library was compiled

	For the two parameters @p f_init and @p f_foreach see function
	#strip_ini_cache().

	The parsing algorithms used by **libconfini** are able to parse any type of file
	encoded in 8-bit code units, as long as the characters that match the regular
	expression `/[\s\[\]\.\\;#"']/` refer to the same code points they refer to in
	ASCII (as they do, for example, in UTF-8 and ISO-8859-1), independently of
	platform-specific conventions.

	@note	In order to be null-byte-injection safe, `NUL` characters, if present in
			the file, will be removed from the dispatched strings.

	Possible return values are: #CONFINI_SUCCESS, #CONFINI_IINTR, #CONFINI_FEINTR,
	#CONFINI_ENOENT, #CONFINI_ENOMEM, #CONFINI_EIO, #CONFINI_EOOR, #CONFINI_EBADF,
	#CONFINI_EFBIG.

	@include topics/load_ini_path.c

**/
int load_ini_path (
	const char * const path,
	const IniFormat format,
	const IniStatsHandler f_init,
	const IniDispHandler f_foreach,
	void * const user_data
) {

	FILE * const ini_file = fopen(path, "rb");

	if (!ini_file) {

		return CONFINI_ENOENT;

	}

	_LIBCONFINI_OFF_T_ file_size;

	if (
		_LIBCONFINI_SEEK_EOF_(ini_file) ||
		(file_size = _LIBCONFINI_FTELL_(ini_file)) < 0
	) {

		return CONFINI_EBADF;

	}

	if (file_size > SIZE_MAX) {

		return CONFINI_EFBIG;

	}

	char * const cache = (char *) malloc((size_t) file_size + 1);

	if (!cache) {

		return CONFINI_ENOMEM;

	}

	rewind(ini_file);

	if (fread(cache, 1, (size_t) file_size, ini_file) < file_size) {

		free(cache);
		return CONFINI_EIO;

	}

	/*  No checks here, as there is nothing we can do about it...  */
	fclose(ini_file);

	const int return_value = strip_ini_cache(
		cache,
		(size_t) file_size,
		format,
		f_init,
		f_foreach,
		user_data
	);

	free(cache);
	return return_value;

}



		/*  OTHER UTILITIES (NOT REQUIRED BY LIBCONFINI'S MAIN FUNCTIONS)  */


											/** @utility{ini_string_match_ss} **/
/**

	@brief			Compare two simple strings and check whether they match
	@param			simple_string_a		The first simple string
	@param			simple_string_b		The second simple string
	@param			format				The format of the INI file
	@return			A boolean: `true` if the two strings match, `false` otherwise

	Simple strings are user-given strings or the result of #ini_string_parse(). The
	@p format argument is used for the following fields:

	- `format.case_sensitive`

**/
bool ini_string_match_ss (
	const char * const simple_string_a,
	const char * const simple_string_b,
	const IniFormat format
) {

	register size_t idx = 0;

	if (format.case_sensitive) {

		do {

			if (simple_string_a[idx] != simple_string_b[idx]) {

				return _LIBCONFINI_FALSE_;

			}

		} while (simple_string_a[idx++]);

		return _LIBCONFINI_TRUE_;

	}

	do {

		if (
			_LIBCONFINI_CHR_CASEFOLD_(simple_string_a[idx]) !=
			_LIBCONFINI_CHR_CASEFOLD_(simple_string_b[idx])
		) {

			return _LIBCONFINI_FALSE_;

		}

	} while (simple_string_a[idx++]);

	return _LIBCONFINI_TRUE_;

}


											/** @utility{ini_string_match_si} **/
/**

	@brief			Compare a simple string and an INI string and and check whether
					they match
	@param			ini_string		The INI string escaped according to
									@p format
	@param			simple_string	The simple string
	@param			format			The format of the INI file
	@return			A boolean: `true` if the two strings match, `false` otherwise

	This function grants that the result of the comparison between a simple string
	and an INI string

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	printf(
		"%s\n",
		ini_string_match_si(my_simple_string, my_ini_string, format) ?
			"They match"
		:
			"They don't match"
	);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	will always match the result of the _literal_ comparison between the simple
	string and the INI string after the latter has been parsed by
	#ini_string_parse() when `format.do_not_collapse_values` is set to `false`.

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	ini_string_parse(my_ini_string, format);

	printf(
		"%s\n",
		ini_string_match_ss(my_simple_string, my_ini_string, format) ?
			"They match"
		:
			"They don't match"
	);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	INI strings are the strings typically dispatched by #load_ini_file(),
	#load_ini_path() or #strip_ini_cache(), which may contain quotes and the three
	escape sequences `\\`, `\'` and `\"`. Simple strings are user-given strings or
	the result of #ini_string_parse().

	In order to be suitable for both names and values, **this function always
	considers sequences of one or more spaces out of quotes in the INI string as
	collapsed**, even when `format.do_not_collapse_values` is set to `true`.

	The @p format argument is used for the following fields:

	- `format.case_sensitive`
	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes` (`INIFORMAT_HAS_NO_ESC()`)

	@include topics/ini_string_match_si.c

**/
bool ini_string_match_si (
	const char * const simple_string,
	const char * const ini_string,
	const IniFormat format
) {

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Format supports escape sequences (const)
		FLAG_8		Unescaped single quotes are odd right now
		FLAG_16		Unescaped double quotes are odd right now
		FLAG_32		This is an escaped single/double quote in a format that supports
					single/double quotes
		FLAG_64		This is a space
		FLAG_128	Skip this character

	*/

	register uint_least8_t
		abcd	=	INIFORMAT_HAS_NO_ESC(format) ?
						67
					:
						68 |
						(format.no_double_quotes << 1) |
						format.no_single_quotes;

	register size_t idx_i = 0;
	size_t idx_s = 0, nbacksl = 0;


	/* \                                /\
	\ */     si_match:                 /* \
	 \/     ______________________     \ */


	if ((abcd & 4) && ini_string[idx_i] == _LIBCONFINI_BACKSLASH_) {

		for (
			abcd &= 63, nbacksl++;
				ini_string[++idx_i] == _LIBCONFINI_BACKSLASH_;
			nbacksl++
		);

	}

	/*  Keep this algorithm identical to #ini_get_bool_i()  */

	abcd	=	!(abcd & 10) && ini_string[idx_i] == _LIBCONFINI_DOUBLE_QUOTES_ ?
					(
						nbacksl & 1 ?
							(abcd & 63) | 32
						:
							((abcd & 223) | 128) ^ 16
					)
				: !(abcd & 17) && ini_string[idx_i] == _LIBCONFINI_SINGLE_QUOTES_ ?
					(
						nbacksl & 1 ?
							(abcd & 63) | 32
						:
							((abcd & 223) | 128) ^ 8
					)
				: (abcd & 24) || !is_some_space(ini_string[idx_i], _LIBCONFINI_WITH_EOL_) ?
					abcd & 31
				: abcd & 64 ?
					(abcd & 223) | 128
				:
					(abcd & 95) | 64;


	if (nbacksl) {

		nbacksl = (abcd & 32 ? nbacksl + 2 : nbacksl + 3) >> 1;

		while (--nbacksl) {

			if (simple_string[idx_s++] != _LIBCONFINI_BACKSLASH_) {

				return _LIBCONFINI_FALSE_;

			}

		}

	}

	if (
		(abcd & 128) || (
			(abcd & 64) && !simple_string[idx_s]
		)
	) {

		idx_i++;
		goto si_match;

	}

	if (
		abcd & 64 ?
			simple_string[idx_s] != _LIBCONFINI_COLLAPSED_ ||
			!simple_string[idx_s + 1]
		: format.case_sensitive ?
			ini_string[idx_i] != simple_string[idx_s]
		:
			_LIBCONFINI_CHR_CASEFOLD_(ini_string[idx_i]) !=
			_LIBCONFINI_CHR_CASEFOLD_(simple_string[idx_s])
	) {

		return _LIBCONFINI_FALSE_;

	}

	idx_s++;

	if (ini_string[idx_i++]) {

		goto si_match;

	}

	return _LIBCONFINI_TRUE_;

}


											/** @utility{ini_string_match_ii} **/
/**

	@brief			Compare two INI strings and check whether they match
	@param			ini_string_a	The first INI string unescaped according to
									@p format
	@param			ini_string_b	The second INI string unescaped according to
									@p format
	@param			format			The format of the INI file
	@return			A boolean: `true` if the two strings match, `false` otherwise

	This function grants that the result of the comparison between two INI strings

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	printf(
		"%s\n",
		ini_string_match_ii(my_ini_string_1, my_ini_string_2, format) ?
			"They match"
		:
			"They don't match"
	);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	will always match the result of the _literal_ comparison between the same two
	INI strings after these have been parsed by #ini_string_parse() when
	`format.do_not_collapse_values` is set to `false`.

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	ini_string_parse(my_ini_string_1, format);
	ini_string_parse(my_ini_string_2, format);

	printf("%s\n",
		ini_string_match_ss(my_ini_string_1, my_ini_string_2, format) ?
			"They match"
		:
			"They don't match"
	);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	INI strings are the strings typically dispatched by #load_ini_file(),
	#load_ini_path() or #strip_ini_cache(), which may contain quotes and the three
	escape sequences `\\`, `\'` and `\"`.

	In order to be suitable for both names and values, **this function always
	considers sequences of one or more spaces out of quotes in both strings as
	collapsed**, even when `format.do_not_collapse_values` is set to `true`.

	The @p format argument is used for the following fields:

	- `format.case_sensitive`
	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes` (`INIFORMAT_HAS_NO_ESC()`)

**/
bool ini_string_match_ii (
	const char * const ini_string_a,
	const char * const ini_string_b,
	const IniFormat format
) {

	const _LIBCONFINI_CHARBOOL_ has_escape = !INIFORMAT_HAS_NO_ESC(format);
	register _LIBCONFINI_CHARBOOL_ side = 1;
	register _LIBCONFINI_CHARBOOL_ turn_allowed = _LIBCONFINI_TRUE_;
	uint_least8_t abcd_pair[2];
	const char * chrptr_pair[2] = { ini_string_a, ini_string_b };
	size_t nbacksl_pair[2];

	/*

	Masks `abcd_pair[0]` and `abcd_pair[1]` (7 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes and format supports
					escape sequences
		FLAG_32		This is a space
		FLAG_64		Skip this character

	*/

	abcd_pair[1] = abcd_pair[0] =
		32 | (format.no_double_quotes << 1) | format.no_single_quotes;


	/* \                                /\
	\ */     ii_match:                 /* \
	 \/     ______________________     \ */


	nbacksl_pair[side] = 0;

	if (has_escape && *chrptr_pair[side] == _LIBCONFINI_BACKSLASH_) {

		for (
			nbacksl_pair[side]++;
				*(++chrptr_pair[side]) == _LIBCONFINI_BACKSLASH_;
			nbacksl_pair[side]++
		);

		abcd_pair[side]		=	nbacksl_pair[side] & 1 ?
									(abcd_pair[side] & 31) | 16
								:
									abcd_pair[side] & 15;

		if (
			(
				(abcd_pair[side] & 9) ||
				*chrptr_pair[side] != _LIBCONFINI_SINGLE_QUOTES_
			) && (
				(abcd_pair[side] & 6) ||
				*chrptr_pair[side] != _LIBCONFINI_DOUBLE_QUOTES_
			)
		) {

			nbacksl_pair[side]++;

		}

	} else {

		abcd_pair[side]	=	!(abcd_pair[side] & 25) &&
							*chrptr_pair[side] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abcd_pair[side] & 111) | 64) ^ 4
							: !(abcd_pair[side] & 22) &&
							*chrptr_pair[side] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abcd_pair[side] & 111) | 64) ^ 8
							: !(abcd_pair[side] & 12) &&
							is_some_space(*chrptr_pair[side], _LIBCONFINI_WITH_EOL_) ?
								(abcd_pair[side] & 111) | 96
							: *chrptr_pair[side] ?
								abcd_pair[side] & 47
							:
								abcd_pair[side] & 15;


		if (abcd_pair[side] & 64) {

			chrptr_pair[side]++;
			goto ii_match;

		}

	}

	if (side && turn_allowed) {

		side ^= 1;
		goto ii_match;

	}

	turn_allowed = _LIBCONFINI_TRUE_;

	if (nbacksl_pair[0] || nbacksl_pair[1]) {

		if (nbacksl_pair[0] >> 1 != nbacksl_pair[1] >> 1) {

			return _LIBCONFINI_FALSE_;

		}

		side = 1;
		goto ii_match;

	}

	if (
		(
			abcd_pair[side ^ 1] & 32 ?
				!(abcd_pair[side] & 32)
			:
				abcd_pair[(side ^= 1) ^ 1] & 32
		) && *chrptr_pair[side]
	) {

		if (*chrptr_pair[side]++ != _LIBCONFINI_COLLAPSED_) {

			return _LIBCONFINI_FALSE_;

		}

		abcd_pair[side ^ 1] &= 95;
		turn_allowed = _LIBCONFINI_FALSE_;
		goto ii_match;

	}

	if (
		format.case_sensitive ?
			*chrptr_pair[0] != *chrptr_pair[1]
		:
			_LIBCONFINI_CHR_CASEFOLD_(*chrptr_pair[0]) !=
			_LIBCONFINI_CHR_CASEFOLD_(*chrptr_pair[1])
	) {

		return _LIBCONFINI_FALSE_;

	}

	if (*chrptr_pair[0]) {

		chrptr_pair[0]++;

	}

	if (*chrptr_pair[1]) {

		chrptr_pair[1]++;

	}

	if (*chrptr_pair[0] || *chrptr_pair[1]) {

		abcd_pair[0] &= 95;
		abcd_pair[1] &= 95;
		side = 1;
		goto ii_match;

	}

	return _LIBCONFINI_TRUE_;

}


												/** @utility{ini_array_match} **/
/**

	@brief			Compare two INI arrays and check whether they match
	@param			ini_string_a	The first INI array
	@param			ini_string_b	The second INI array
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			A boolean: `true` if the two arrays match, `false` otherwise

	This function grants that the result of the comparison between two INI arrays
	will always match the the _literal_ comparison between the individual members
	of both arrays after these have been parsed, one by one, by #ini_string_parse()
	(with `format.do_not_collapse_values` set to `false`).

	This function can be used, with `'.'` as delimiter, to compare section paths.

	INI strings are the strings typically dispatched by #load_ini_file(),
	#load_ini_path() or #strip_ini_cache(), which may contain quotes and the three
	escape sequences `\\`, `\'` and `\"`.

	In order to be suitable for both names and values, **this function always
	considers sequences of one or more spaces out of quotes in both strings as
	collapsed**, even when `format.do_not_collapse_values` is set to `true`.

	The @p format argument is used for the following fields:

	- `format.case_sensitive`
	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes` (`INIFORMAT_HAS_NO_ESC()`)

**/
bool ini_array_match (
	const char * const ini_string_a,
	const char * const ini_string_b,
	const char delimiter,
	const IniFormat format
) {

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*  We have no delimiters (array has only one member)  */

		return ini_string_match_ii(ini_string_a, ini_string_b, format);

	}

	const _LIBCONFINI_CHARBOOL_ has_escape = !INIFORMAT_HAS_NO_ESC(format);
	register _LIBCONFINI_CHARBOOL_ side = 1;
	register _LIBCONFINI_CHARBOOL_ turn_allowed = _LIBCONFINI_TRUE_;
	uint_least8_t abcd_pair[2];
	size_t nbacksl_pair[2];
	const char * chrptr_pair[2] = {
		ini_string_a + ltrim_s(ini_string_a, 0, _LIBCONFINI_WITH_EOL_),
		ini_string_b + ltrim_s(ini_string_b, 0, _LIBCONFINI_WITH_EOL_)
	};

	/*

	Masks `abcd_pair[0]` and `abcd_pair[1]` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes and format supports
					escape sequences
		FLAG_32		This is a space
		FLAG_64		This is a delimiter
		FLAG_128	Skip this character

	*/

	abcd_pair[1] = abcd_pair[0] =
		32 | (format.no_double_quotes << 1) | format.no_single_quotes;


	/* \                                /\
	\ */     delimited_match:          /* \
	 \/     ______________________     \ */


	nbacksl_pair[side] = 0;

	if (has_escape && *chrptr_pair[side] == _LIBCONFINI_BACKSLASH_) {

		for (
			nbacksl_pair[side]++;
				*(++chrptr_pair[side]) == _LIBCONFINI_BACKSLASH_;
			nbacksl_pair[side]++
		);

		abcd_pair[side]		=	nbacksl_pair[side] & 1 ?
									(abcd_pair[side] & 31) | 16
								:
									abcd_pair[side] & 15;

		if (
			(
				(abcd_pair[side] & 9) ||
				*chrptr_pair[side] != _LIBCONFINI_SINGLE_QUOTES_
			) && (
				(abcd_pair[side] & 6) ||
				*chrptr_pair[side] != _LIBCONFINI_DOUBLE_QUOTES_
			)
		) {

			nbacksl_pair[side]++;

		}

	} else {

		abcd_pair[side] =
			!(abcd_pair[side] & 12) &&
			is_some_space(*chrptr_pair[side], _LIBCONFINI_WITH_EOL_) ?
				(
					delimiter || (abcd_pair[side] & 64) ?
						(abcd_pair[side] & 239) | 160
					:
						(abcd_pair[side] & 111) | 96
				)
			: delimiter && !(abcd_pair[side] & 12) && *chrptr_pair[side] == delimiter ?
				(abcd_pair[side] & 111) | 96
			: !(abcd_pair[side] & 25) && *chrptr_pair[side] == _LIBCONFINI_SINGLE_QUOTES_ ?
				((abcd_pair[side] & 175) | 128) ^ 4
			: !(abcd_pair[side] & 22) && *chrptr_pair[side] == _LIBCONFINI_DOUBLE_QUOTES_ ?
				((abcd_pair[side] & 175) | 128) ^ 8
			: *chrptr_pair[side] ?
				abcd_pair[side] & 47
			: delimiter ?
				abcd_pair[side] & 15
			:
				(abcd_pair[side] & 79) ^ 64;


		if (abcd_pair[side] & 128) {

			chrptr_pair[side]++;
			goto delimited_match;

		}

	}

	if (side && turn_allowed) {

		side ^= 1;
		goto delimited_match;

	}

	turn_allowed = _LIBCONFINI_TRUE_;

	if (nbacksl_pair[0] || nbacksl_pair[1]) {

		if (nbacksl_pair[0] >> 1 != nbacksl_pair[1] >> 1) {

			return _LIBCONFINI_FALSE_;

		}

		side = 1;
		goto delimited_match;

	}

	if ((abcd_pair[0] ^ abcd_pair[1]) & 64) {

		return _LIBCONFINI_FALSE_;

	}

	if (
		!(
			abcd_pair[side ^ 1] & 32 ?
				abcd_pair[side] & 96
			:
				(abcd_pair[(side ^= 1) ^ 1] & 96) ^ 32
		) && *chrptr_pair[side]
	) {

		if (*chrptr_pair[side]++ != _LIBCONFINI_COLLAPSED_) {

			return _LIBCONFINI_FALSE_;

		}

		abcd_pair[side ^ 1] &= 223;
		turn_allowed = _LIBCONFINI_FALSE_;
		goto delimited_match;

	}

	if (~abcd_pair[0] & 64) {

		if (
			format.case_sensitive ?
				*chrptr_pair[0] != *chrptr_pair[1]
			:
				_LIBCONFINI_CHR_CASEFOLD_(*chrptr_pair[0]) !=
				_LIBCONFINI_CHR_CASEFOLD_(*chrptr_pair[1])
		) {

			return _LIBCONFINI_FALSE_;

		}

		abcd_pair[0] &= 223;
		abcd_pair[1] &= 223;

	}

	if (*chrptr_pair[0]) {

		chrptr_pair[0]++;

	}

	if (*chrptr_pair[1]) {

		chrptr_pair[1]++;

	}

	if (*chrptr_pair[0] || *chrptr_pair[1]) {

		side = 1;
		goto delimited_match;

	}

	return _LIBCONFINI_TRUE_;

}


													/** @utility{ini_unquote} **/
/**

	@brief			Unescape `\'`, `\"`, and `\\` and remove all unescaped quotes
					(when single/double quotes are considered metacharacters in
					respect to the format given)
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

	This function is very similar to #ini_string_parse(), except that does not
	bother collapsing the sequences of more than one space that might result from
	removing empty quotes. Its purpose is to be used to parse key and section names,
	since these are always dispatched as already collapsed. In order to parse
	values, or array parts listed in values, please use #ini_string_parse().

	If you only need to compare @p ini_string with another string, consider to use
	#ini_string_match_si() and #ini_string_match_ii() instead of parsing the former
	and perform a simple comparison afterwards. These two functions are in fact able
	to check directly for equality between unparsed INI strings without actually
	modifiyng them.

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well). If the string does not contain quotes, or if quotes are
	considered to be normal characters, no changes will be made.

	@note	If @p ini_string comes from #INI_GLOBAL_IMPLICIT_VALUE this function is
			no-op and will only return the value of #INI_GLOBAL_IMPLICIT_V_LEN minus
			the offset of @p ini_string within #INI_GLOBAL_IMPLICIT_VALUE.

	The @p format argument is used for the following fields:

	- `format.no_single_quotes`
	- `format.no_double_quotes`
	- `format.multiline_nodes` (`INIFORMAT_HAS_NO_ESC()`)

	@include topics/ini_string_parse.c

**/
size_t ini_unquote (char * const ini_string, const IniFormat format) {

	if (INI_IS_IMPLICIT_SUBSTR(ini_string)) {

		return INI_GLOBAL_IMPLICIT_V_LEN + INI_GLOBAL_IMPLICIT_VALUE - ini_string;

	}

	register size_t idx = 0;

	if (INIFORMAT_HAS_NO_ESC(format)) {

		/*

			There are no escape sequences... I will just return the length of the
			string...

		*/

		while (ini_string[idx++]);

		return idx - 1;

	}

	size_t lshift = 0, nbacksl = 0;

	/*

	Mask `abcd` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		This is an unescaped single quote and format supports single
					quotes
		FLAG_32		This is an unescaped double quote and format supports double
					quotes

	*/

	for (

		register uint_least8_t
			abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

			ini_string[idx];

		idx++

	) {

		abcd	=	!(abcd & 6) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(abcd & 47) | 32
					: !(abcd & 9) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(abcd & 31) | 16
					:
						abcd & 15;


		if (!(nbacksl & 1) && (abcd & 48)) {

			abcd ^= (abcd >> 2) & 12;
			lshift++;
			continue;

		}

		if (ini_string[idx] == _LIBCONFINI_BACKSLASH_) {

			nbacksl++;

		} else {

			if ((nbacksl & 1) && (abcd & 48)) {

				lshift++;

			}

			lshift += nbacksl >> 1;
			nbacksl = 0;

		}

		if (lshift) {

			ini_string[idx - lshift] = ini_string[idx];

		}

	}

	lshift += nbacksl >> 1;

	for (idx -= lshift; ini_string[idx]; ini_string[idx++] = '\0');

	return idx - lshift;

}


												/** @utility{ini_string_parse} **/
/**

	@brief			Unescape `\'`, `\"`, and `\\` and remove all unescaped quotes
					(when single/double quotes are considered metacharacters in
					respect to the format given); if the format allows it, sequences
					of one or more spaces out of quotes will be collapsed
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

	This function is meant to be used to parse values. In order to parse key and
	section names please use #ini_unquote().

	If you only need to compare @p ini_string with another string, consider to use
	#ini_string_match_si() and #ini_string_match_ii() instead of parsing the former
	and perform a simple comparison afterwards. These two functions are in fact able
	to check directly for equality between unparsed INI strings without actually
	modifying them.

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well). If `format.do_not_collapse_values` is set to non-zero, spaces
	surrounding empty quotes will be collapsed together with the latter.

	@note	If @p ini_string comes from #INI_GLOBAL_IMPLICIT_VALUE this function is
			no-op and will only return the value of #INI_GLOBAL_IMPLICIT_V_LEN minus
			the offset of @p ini_string within #INI_GLOBAL_IMPLICIT_VALUE.

	The @p format argument is used for the following fields:

	- `format.no_single_quotes`
	- `format.no_double_quotes`
	- `format.multiline_nodes` (`INIFORMAT_HAS_NO_ESC()`)
	- `format.do_not_collapse_values`

	@include topics/ini_string_parse.c

**/
size_t ini_string_parse (char * const ini_string, const IniFormat format) {

	if (INI_IS_IMPLICIT_SUBSTR(ini_string)) {

		return INI_GLOBAL_IMPLICIT_V_LEN + INI_GLOBAL_IMPLICIT_VALUE - ini_string;

	}

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Do not collapse spaces within members (const)
		FLAG_8		Unescaped single quotes are odd right now
		FLAG_16		Unescaped double quotes are odd right now
		FLAG_32		This is an *escaped* single/double quote and format supports
					single/double quotes
		FLAG_64		This is a space
		FLAG_128	Skip this character

	*/

	register uint_least8_t
		abcd	=	(format.do_not_collapse_values ? 68 : 64) |
					(format.no_double_quotes << 1) |
					format.no_single_quotes;

	size_t idx, lshift;

	if (format.multiline_nodes == INI_NO_MULTILINE) {

		switch (abcd) {

			case 64 | 2 | 1 :

				/*

					There are no escape sequences, but spaces might still need to
					be collapsed.

				*/

				for (idx = 0, lshift = 0; ini_string[idx]; idx++) {

					abcd	=	!is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_) ?
									3
								: abcd & 64 ?
									195
								:
									67;

					if (abcd & 128) {

						lshift++;

					} else {

						ini_string[idx - lshift]	=	abcd & 64 ?
															_LIBCONFINI_COLLAPSED_
														:
															ini_string[idx];

					}

				}

				for (

					idx	-=	(abcd & 64) && lshift < idx ?
								++lshift
							:
								lshift;

						ini_string[idx];

					ini_string[idx++] = '\0'

				);

				return idx - lshift;

			case 64 | 4 | 2 | 1 :

				/*

					There are no escape sequences and spaces do not need to be
					collapsed, but left and right trim might still be necessary.

				*/

				return rtrim_h(
					ini_string,
					ltrim_hh(ini_string, 0, _LIBCONFINI_WITH_EOL_),
					_LIBCONFINI_WITH_EOL_
				);

		}

	}

	/*

		There might be escape sequences...

	*/

	size_t nbacksl = 0;

	for (idx = lshift = 0; ini_string[idx]; idx++) {

		abcd	=	!(abcd & 10) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							nbacksl & 1 ?
								(abcd & 63) | 32
							:
								((abcd & 223) | 128) ^ 16
						)
					: !(abcd & 17) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							nbacksl & 1 ?
								(abcd & 63) | 32
							:
								((abcd & 223) | 128) ^ 8
						)
					: (abcd & 28) || !is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_) ?
						abcd & 31
					: abcd & 64 ?
						(abcd & 223) | 128
					:
						(abcd & 95) | 64;


		if (abcd & 128) {

			lshift++;
			continue;

		}

		if (ini_string[idx] == _LIBCONFINI_BACKSLASH_) {

			nbacksl++;

		} else {

			if (abcd & 32) {

				lshift++;

			}

			lshift += nbacksl >> 1;
			nbacksl = 0;

		}

		ini_string[idx - lshift]	=	abcd & 64 ?
											_LIBCONFINI_COLLAPSED_
										:
											ini_string[idx];

	}

	lshift += nbacksl >> 1;

	for (

		idx	-=	(abcd & 64) && lshift < idx ?
					++lshift
				:
					lshift;

			ini_string[idx];

		ini_string[idx++] = '\0'

	);

	return	(abcd & 28) ^ 4 ?
				idx - lshift
			:
				rtrim_h(ini_string, idx - lshift, _LIBCONFINI_WITH_EOL_);

}


											/** @utility{ini_array_get_length} **/
/**

	@brief			Get the length of a stringified INI array in number of members
	@param			ini_string		The stringified array (it can be `NULL`)
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The length of the INI array in number of members

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

**/
size_t ini_array_get_length (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format
) {

	if (!ini_string) {

		return 0;

	}

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*  We have no delimiters (array has only one member)  */

		return 1;

	}

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Delimiter is not any space (const)
		FLAG_8		Unescaped single quotes are odd right now
		FLAG_16		Unescaped double quotes are odd right now
		FLAG_32		We are in an odd sequence of backslashes
		FLAG_64		This is a space
		FLAG_128	This is a delimiter

	*/

	register uint_least8_t
		abcd	=	(delimiter ? 64 : 68) |
					(format.no_double_quotes << 1) |
					format.no_single_quotes;

	size_t counter = 0;

	for (size_t idx = 0; ini_string[idx]; idx++) {

		/*  Revision #1  */

		abcd	=	!(abcd & 28) && ini_string[idx] == delimiter ?
						(abcd & 159) | 128
					: !(abcd & 24) && is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_) ?
						(
							(abcd & 68) ^ 4 ?
								(abcd & 95) | 64
							:
								(abcd & 223) | 192
						)
					: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd & 63) ^ 32
					: !(abcd & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(abcd & 31) ^ 16
					: !(abcd & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(abcd & 31) ^ 8
					:
						abcd & 31;


		if (abcd & 128) {

			counter++;

		}

	}

	return	!counter || (~abcd & 68) ?
				counter + 1
			:
				counter;

}


												/** @utility{ini_array_foreach} **/
/**

	@brief			Call a custom function for each member of a stringified INI
					array, without modifying the content of the buffer -- useful for
					read-only (`const`) stringified arrays
	@param			ini_string		The stringified array (it can be `NULL`)
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@param			f_foreach		The function that will be invoked for each array
									member
	@param			user_data		A custom argument, or `NULL`
	@return			Zero for success, otherwise an error code (see `enum`
					#ConfiniInterruptNo)

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	The user given function @p f_foreach (see #IniSubstrHandler data type) will be
	invoked with six arguments: `ini_string`, `memb_offset` (the offset of the
	member in bytes), `memb_length` (the length of the member in bytes), `memb_num`
	(the offset of the member in number of members), `format` (the format of the INI
	file), `user_data` (the custom argument @p user_data previously passed). If
	@p f_foreach returns a non-zero value the function #ini_array_foreach() will be
	interrupted.

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	Possible return values are: #CONFINI_SUCCESS, #CONFINI_FEINTR.

	@include topics/ini_array_foreach.c

**/
int ini_array_foreach (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format,
	const IniSubstrHandler f_foreach,
	void * const user_data
) {

	if (!ini_string) {

		return CONFINI_SUCCESS;

	}

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Delimiter is not any space (const)
		FLAG_8		Unescaped single quotes are odd until now
		FLAG_16		Unescaped double quotes are odd until now
		FLAG_32		We are in an odd sequence of backslashes
		FLAG_64		This is not a delimiter
		FLAG_128	Stop the loop

	*/

	register size_t idx;
	size_t offs = ltrim_s(ini_string, 0, _LIBCONFINI_WITH_EOL_);

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*  We have no delimiters (array has only one member)  */

		idx = 0;

		while (ini_string[idx++]);

		return f_foreach(
			ini_string,
			offs,
			rtrim_s(ini_string + offs, idx - offs - 1, _LIBCONFINI_WITH_EOL_),
			0,
			format,
			user_data
		) ? CONFINI_FEINTR : CONFINI_SUCCESS;

	}

	register uint_least8_t
		abcd	=	(delimiter ? 4 : 0) |
					(format.no_double_quotes << 1) |
					format.no_single_quotes;

	size_t memb_num = 0;

	idx = offs;

	do {

		abcd	=	(
						delimiter ?
							ini_string[idx] == delimiter
						:
							is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_)
					) ?
						abcd & 159
					: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd | 64) ^ 32
					: !(abcd & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						((abcd & 223) | 64) ^ 16
					: !(abcd & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						((abcd & 223) | 64) ^ 8
					: ini_string[idx] ?
						(abcd & 223) | 64
					:
						128;


		if (!(abcd & 88)) {

			if (
				f_foreach(
					ini_string,
					offs,
					rtrim_s(ini_string + offs, idx - offs, _LIBCONFINI_WITH_EOL_),
					memb_num++,
					format,
					user_data
				)
			) {

				return CONFINI_FEINTR;

			}

			offs	=	abcd & 128 ?
							idx + 1
						:
							ltrim_s(ini_string, idx + 1, _LIBCONFINI_WITH_EOL_);

		}

		idx = abcd & 216 ? idx + 1 : offs;

	} while (
		!(abcd & 128) && (
			(abcd & 92) || ini_string[idx]
		)
	);

	return CONFINI_SUCCESS;

}


												/** @utility{ini_array_shift} **/
/**

	@brief			Shift the location pointed by @p ini_strptr to the next member
					of the INI array (without modifying the content of the buffer),
					or to `NULL` if the INI array has no more members -- useful for
					read-only (`const`) stringified arrays
	@param			ini_strptr		The memory location of the stringified array --
									it cannot be `NULL`, but it can point to `NULL`
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The length of the array member that has been left behind

	Usually @p ini_strptr comes from an #IniDispatch (but any other string may be
	used as well).

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_shift.c

**/
size_t ini_array_shift (
	const char ** const ini_strptr,
	const char delimiter,
	const IniFormat format
) {

	size_t toklen = 0;

	if (*ini_strptr && !_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		if (!delimiter) {

			toklen = ltrim_s(*ini_strptr, 0, _LIBCONFINI_WITH_EOL_);

		}

		toklen += get_metachar_pos(*ini_strptr + toklen, delimiter, format);
		*ini_strptr += toklen;
		toklen = rtrim_s(*ini_strptr - toklen, toklen, _LIBCONFINI_WITH_EOL_);

		if (**ini_strptr) {

			*ini_strptr += ltrim_s(*ini_strptr, 1, _LIBCONFINI_WITH_EOL_);

			if (delimiter || **ini_strptr) {

				return toklen;

			}

		}

	}

	*ini_strptr = (char *) 0;
	return toklen;

}


												/** @utility{ini_array_collapse} **/
/**

	@brief			Compress the distribution of the data in a stringified INI array
					by removing all the white spaces that surround its delimiters,
					empty quotes, collapsable spaces, etc
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members --
									if zero (`INI_ANY_SPACE`) any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The new length of the stringified array

	Out of quotes similar to ECMAScript
	`ini_string.replace(new RegExp("^\\s+|\\s*(?:(" + delimiter + ")\\s*|($))", "g"), "$1$2")`.
	If #INI_ANY_SPACE (`0`) is used as delimiter, one or more different spaces
	(`/[\t \v\f\n\r]+/`) will be always collapsed to one space, independently of
	what the format says.

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	This function can be useful before invoking `memcpy()` using @p ini_string as
	source, when saving memory is a priority.

	The @p format argument is used for the following fields:

	- `format.no_single_quotes`
	- `format.no_double_quotes`
	- `format.do_not_collapse_values`
	- `format.preserve_empty_quotes`

	Examples:

	1. Using comma as delimiter:
	   - Before: `&nbsp;first&nbsp;&nbsp; ,&nbsp;&nbsp;&nbsp; second&nbsp;&nbsp;
	     ,&nbsp;&nbsp; third&nbsp;&nbsp; ,&nbsp; etc.&nbsp;&nbsp;`
	   - After: `first,second,third,etc.`
	2. Using `INI_ANY_SPACE` as delimiter:
	   - Before: `&nbsp;&nbsp;first&nbsp;&nbsp;&nbsp; second&nbsp;&nbsp;&nbsp;
	     third&nbsp;&nbsp;&nbsp;&nbsp; etc.&nbsp;&nbsp;&nbsp;`
	   - After: `first second third etc.`

	@note	If @p ini_string comes from #INI_GLOBAL_IMPLICIT_VALUE this function is
			no-op and will only return the value of #INI_GLOBAL_IMPLICIT_V_LEN minus
			the offset of @p ini_string within #INI_GLOBAL_IMPLICIT_VALUE.

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_collapse.c

	@note	The actual space occupied by the array might get reduced further after
			each member is parsed by #ini_string_parse().

**/
size_t ini_array_collapse (
	char * const ini_string,
	const char delimiter,
	const IniFormat format
) {

	if (INI_IS_IMPLICIT_SUBSTR(ini_string)) {

		return INI_GLOBAL_IMPLICIT_V_LEN + INI_GLOBAL_IMPLICIT_VALUE - ini_string;

	}

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*  We have no delimiters (array has only one member)  */

		switch (
			(format.preserve_empty_quotes << 1) |
			format.do_not_collapse_values
		) {

			case 0: return collapse_everything(ini_string, format);
			case 1: return collapse_empty_quotes(ini_string, format);
			case 2: return collapse_spaces(ini_string, format);
			case 3: return rtrim_h(
						ini_string,
						ltrim_hh(ini_string, 0, _LIBCONFINI_WITH_EOL_),
						_LIBCONFINI_WITH_EOL_
					);

		}

	}

	/*

	Mask `abcd` (16 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Do not collapse spaces within members (const)
		FLAG_8		Preserve empty quotes (const)
		FLAG_16		Any space is delimiter (const)
		FLAG_32		Unescaped single quotes are odd right now
		FLAG_64		Unescaped double quotes are odd right now
		FLAG_128	We are in an odd sequence of backslashes
		FLAG_256	This is *not* a delimiter out of quotes
		FLAG_512	This is *not* a space out of quotes
		FLAG_1024	These are some quotes
		FLAG_2048	These are some quotes or among the last spaces are some empty
					quotes
		FLAG_4096	Save current `idx_d` in `fallback`
		FLAG_8192	Restore `idx_d` from `fallback` before writing
		FLAG_16384	Decrease `idx_d` before writing
		FLAG_32768	Keep increasing `idx_d` after writing

	*/

	size_t idx_s = 0, idx_d = 0, fallback = 0;

	register uint_least16_t
		abcd	=	(delimiter ? 0 : 16) |
					(format.preserve_empty_quotes << 3) |
					(format.do_not_collapse_values << 2) |
					(format.no_double_quotes << 1) |
					format.no_single_quotes;


	for (; ini_string[idx_s]; idx_s++) {

		/*  Revision #1  */

		abcd	=	!(abcd & 112) && ini_string[idx_s] == delimiter ?
						(
							(abcd & 536) && ((abcd & 1560) ^ 8) &&
							((abcd & 1560) ^ 1544) && ((abcd & 1304) ^ 1032) ?
								(abcd & 33407) | 33280
							:
								(abcd & 41599) | 41472
						)
					: !(abcd & 96) && is_some_space(ini_string[idx_s], _LIBCONFINI_WITH_EOL_) ?
						(
							!((abcd & 1816) ^ 1800) ?
								(abcd & 43391) | 40960
							: !(~abcd & 1560) ?
								(abcd & 41087) | 40960
							: !((abcd & 536) ^ 528) || !((abcd & 1560) ^ 536) ||
							!((abcd & 1560) ^ 1048) ?
								(abcd & 32895) | 32768
							: !(abcd & 540) || !((abcd & 1564) ^ 8) ||
							!((abcd & 536) ^ 16) || !((abcd & 1560) ^ 24) ?
								abcd & 2431
							: ((abcd & 540) ^ 4) && ((abcd & 796) ^ 12) &&
							((abcd & 1564) ^ 12) && ((abcd & 1308) ^ 1032) ?
								(abcd & 39295) | 36864
							:
								(abcd & 35199) | 32768
						)
					: !(abcd & 193) && ini_string[idx_s] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							!((abcd & 3896) ^ 8) ?
								(abcd & 44927) | 44064
							: !((abcd & 3896) ^ 2056) ?
								(abcd & 36735) | 36128
							: !((abcd & 1056) ^ 32) ?
								(abcd & 33631) | 33536
							: !(abcd & 40) || !(~abcd & 1064) ?
								((abcd & 36735) | 35840) ^ 32
							: ((abcd & 1064) ^ 1032) && ((abcd & 1064) ^ 1056) ?
								(abcd & 40831) | 39968
							:
								((abcd & 20351) | 19456) ^ 32
						)
					: !(abcd & 162) && ini_string[idx_s] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							!((abcd & 3928) ^ 8) ?
								(abcd & 44927) | 44096
							: !((abcd & 3928) ^ 2056) ?
								(abcd & 36735) | 36160
							: !((abcd & 1088) ^ 64) ?
								(abcd & 33599) | 33536
							: !(abcd & 72) || !(~abcd & 1096) ?
								((abcd & 36735) | 35840) ^ 64
							: ((abcd & 1096) ^ 1088) && ((abcd & 1096) ^ 1032) ?
								(abcd & 40831) | 40000
							:
								((abcd & 20351) | 19456) ^ 64
						)
					: ini_string[idx_s] == _LIBCONFINI_BACKSLASH_ ?
						(
							(abcd & 888) && ((abcd & 1144) ^ 1032) &&
							((abcd & 1144) ^ 1048) && ((abcd & 2936) ^ 8) ?
								((abcd & 33791) | 33536) ^ 128
							:
								((abcd & 41983) | 41728) ^ 128
						)
					: (abcd & 888) && ((abcd & 1144) ^ 1032) &&
					((abcd & 1144) ^ 1048) && ((abcd & 2936) ^ 8) ?
						(abcd & 33663) | 33536
					:
						(abcd & 41855) | 41728;


		ini_string[
			abcd & 16384 ?
				--idx_d
			: abcd & 8192 ?
				(idx_d = fallback)
			: abcd & 4096 ?
				(fallback = idx_d)
			:
				idx_d
		]							=	(abcd & 1636) && ((abcd & 1392) ^ 16) ?
											ini_string[idx_s]
										:
											_LIBCONFINI_COLLAPSED_;


		if (abcd & 32768) {

			idx_d++;

		}

	}

	for (

		idx_s	=	((abcd & 16) && !idx_d) || (!(~abcd & 1040) && idx_d < 4) ?
						(idx_d = 0)
					: !(abcd & 536) || !(~abcd & 1544) || !((abcd & 1560) ^ 8) ||
					!((abcd & 1304) ^ 1032) ?
						(idx_d = fallback)
					: !((abcd & 1624) ^ 1104) || !((abcd & 1592) ^ 1072) ?
						(idx_d -= 2)
					: ((abcd & 1552) ^ 16) && ((abcd & 632) ^ 16) &&
					((abcd & 1624) ^ 1616) && ((abcd & 1592) ^ 1584) ?
						idx_d
					:
						--idx_d;

			ini_string[idx_s];

		ini_string[idx_s++] = '\0'

	);

	return idx_d;

}


												/** @utility{ini_array_break} **/
/**

	@brief			Replace the first delimiter found (together with the spaces that
					surround it) with `\0`
	@param			ini_string		The stringified array (it can be `NULL`)
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			A pointer to the remaining INI array or `NULL` if the remaining
					array is empty

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	Similarly to `strtok_r()` this function can be used only once for a given
	string.

	@note	If @p ini_string comes from #INI_GLOBAL_IMPLICIT_VALUE this function is
			no-op and will return `NULL`.

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_break.c

**/
char * ini_array_break (
	char * const ini_string,
	const char delimiter,
	const IniFormat format
) {

	if (ini_string && !INI_IS_IMPLICIT_SUBSTR(ini_string)) {

		char * remnant;

		if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

			/*  We have no delimiters (array has only one member)  */

			remnant = ini_string;

			while (*remnant++);

			rtrim_h(ini_string, remnant - ini_string - 1, _LIBCONFINI_WITH_EOL_);

		} else {

			remnant = ini_string + get_metachar_pos(ini_string, delimiter, format);

			if (*remnant) {

				*remnant = '\0';
				rtrim_h(ini_string, remnant - ini_string, _LIBCONFINI_WITH_EOL_);
				remnant += ltrim_h(remnant, 1, _LIBCONFINI_WITH_EOL_);

				if (delimiter || *remnant) {

					return remnant;

				}

			}

		}

	}

	return (char *) 0;

}


												/** @utility{ini_array_release} **/
/**

	@brief			Replace the first delimiter found (together with the spaces that
					surround it) with `\0`, then shifts the location pointed by
					@p ini_strptr to the next member of the INI array, or to `NULL`
					if the INI array has no more members
	@param			ini_strptr		The memory location of the stringified array --
									it cannot be `NULL`, but it can point to `NULL`
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The array member that has been released

	Usually @p ini_strptr comes from an #IniDispatch (but any other string may be
	used as well).

	Similarly to `strtok_r()` this function can be used only once for a given
	string.

	@note	If @p ini_string comes from #INI_GLOBAL_IMPLICIT_VALUE this function is
			no-op and will set @p ini_strptr to `NULL`.

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_release.c

**/
char * ini_array_release (
	char ** const ini_strptr,
	const char delimiter,
	const IniFormat format
) {

	char * const token = *ini_strptr;

	if (
		token &&
		!INI_IS_IMPLICIT_SUBSTR(token) &&
		!_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)
	) {

		*ini_strptr += get_metachar_pos(*ini_strptr, delimiter, format);

		if (**ini_strptr) {

			**ini_strptr = '\0';
			rtrim_h(token, *ini_strptr - token, _LIBCONFINI_WITH_EOL_);
			*ini_strptr += ltrim_h(*ini_strptr, 1, _LIBCONFINI_WITH_EOL_);

			if (delimiter || **ini_strptr) {

				return token;

			}

		}

	}

	*ini_strptr = (char *) 0;
	return token;

}


												/** @utility{ini_array_split} **/
/**

	@brief			Split a stringified INI array into NUL-separated members and
					call a custom function for each member
	@param			ini_string		The stringified array (it cannot be `NULL`)
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@param			f_foreach		The function that will be invoked for each array
									member
	@param			user_data		A custom argument, or `NULL`
	@return			Zero for success, otherwise an error code (see `enum`
					#ConfiniInterruptNo)

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	The user given function @p f_foreach (see #IniStrHandler data type) will be
	invoked with five arguments: `member` (the member of the array), `memb_length`
	(the length of the member in bytes), `memb_num` (the offset of the member in
	number of members), `format` (the format of the INI file), `user_data` (the
	custom argument @p user_data previously passed). If @p f_foreach returns a
	non-zero value the function #ini_array_split() will be interrupted.

	Similarly to `strtok_r()` this function can be used only once for a given
	string.

	@note	If @p ini_string comes from #INI_GLOBAL_IMPLICIT_VALUE or is `NULL` this
			function is no-op and will return an error code.

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	Possible return values are: #CONFINI_SUCCESS, #CONFINI_EROADDR, #CONFINI_FEINTR.

	@include topics/ini_array_split.c

**/
int ini_array_split (
	char * const ini_string,
	const char delimiter,
	const IniFormat format,
	const IniStrHandler f_foreach,
	void * const user_data
) {

	if (!ini_string || INI_IS_IMPLICIT_SUBSTR(ini_string)) {

		return CONFINI_EROADDR;

	}

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Delimiter is not any space (const)
		FLAG_8		Unescaped single quotes are odd until now
		FLAG_16		Unescaped double quotes are odd until now
		FLAG_32		We are in an odd sequence of backslashes
		FLAG_64		This is not a delimiter
		FLAG_128	Stop the loop

	*/

	register size_t idx;
	size_t offs = ltrim_h(ini_string, 0, _LIBCONFINI_WITH_EOL_);

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*  We have no delimiters (array has only one member)  */

		idx = 0;

		while (ini_string[idx++]);

		return f_foreach(
			ini_string + offs,
			rtrim_h(ini_string + offs, idx - offs - 1, _LIBCONFINI_WITH_EOL_),
			0,
			format,
			user_data
		) ? CONFINI_FEINTR : CONFINI_SUCCESS;

	}

	register uint_least8_t
		abcd	=	(delimiter ? 4 : 0) |
					(format.no_double_quotes << 1) |
					format.no_single_quotes;

	size_t memb_num = 0;

	idx = offs;


	do {

		abcd	=	(
						delimiter ?
							ini_string[idx] == delimiter
						:
							is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_)
					) ?
						abcd & 159
					: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd | 64) ^ 32
					: !(abcd & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						((abcd & 223) | 64) ^ 16
					: !(abcd & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						((abcd & 223) | 64) ^ 8
					: ini_string[idx] ?
						(abcd & 223) | 64
					:
						128;


		if (!(abcd & 88)) {

			ini_string[idx] = '\0';

			if (
				f_foreach(
					ini_string + offs,
					rtrim_h(ini_string + offs, idx - offs, _LIBCONFINI_WITH_EOL_),
					memb_num++,
					format,
					user_data
				)
			) {

				return CONFINI_FEINTR;

			}

			offs	=	abcd & 128 ?
							idx + 1
						:
							ltrim_h(ini_string, idx + 1, _LIBCONFINI_WITH_EOL_);

		}

		idx = abcd & 216 ? idx + 1 : offs;

	} while (
		!(abcd & 128) && (
			(abcd & 92) || ini_string[idx]
		)
	);

	return CONFINI_SUCCESS;

}


/**

	@brief			Set the value of the global variable
					#INI_GLOBAL_LOWERCASE_MODE
	@deprecated		Deprecated since version 1.15.0 (it will be removed in version
					2.0.0)
	@param			lowercase		The new value
	@return			Nothing

	If @p lowercase is `true`, key and section names in case-insensitive INI formats
	will be dispatched lowercase, verbatim otherwise (default value: `true`).

	@warning	This function changes the value of one or more global variables. In
				order to be thread-safe this function should be used only once at
				beginning of execution, or otherwise a mutex logic must be
				introduced.

**/
void ini_global_set_lowercase_mode (const bool lowercase) {

	INI_GLOBAL_LOWERCASE_MODE = lowercase;

}


									/** @utility{ini_global_set_implicit_value} **/
/**

	@brief			Set the value to be to be assigned to implicit keys
	@param			implicit_value		The string to be used as implicit value
										(usually `"YES"`, `"TRUE"`, or `"ON"`, or
										any other string; it can be `NULL`)
	@param			implicit_v_len		The length of @p implicit_value (without
										counting the NUL terminator; use `0` for
										both an empty string and `NULL`)
	@return			Nothing

	@warning	This function changes the value of one or more global variables. In
				order to be thread-safe this function should be used only once at
				beginning of execution, or otherwise a mutex logic must be
				introduced.

	@include topics/ini_global_set_implicit_value.c

**/
void ini_global_set_implicit_value (
	char * const implicit_value,
	const size_t implicit_v_len
) {

	INI_GLOBAL_IMPLICIT_VALUE = implicit_value;
	INI_GLOBAL_IMPLICIT_V_LEN = implicit_v_len;

}


														/** @utility{ini_fton} **/
/**

	@brief			Calculate the #IniFormatNum of an #IniFormat
	@param			source			The #IniFormat to compute
	@return			The unique unsigned integer that identifies the format given

**/
IniFormatNum ini_fton (const IniFormat source) {

	#define __INIFORMAT_ID__(NAME, OFFSET, SIZE, DEFVAL) (source.NAME << OFFSET) |

	return INIFORMAT_TABLE_AS(__INIFORMAT_ID__) 0;

	#undef __INIFORMAT_ID__

}


														/** @utility{ini_ntof} **/
/**

	@brief			Construct a new #IniFormat according to an #IniFormatNum
	@param			format_num		The #IniFormatNum to parse
	@return			The new #IniFormat constructed

	@note	If @p format_num `>` `16777215` it will be truncated to 24 bits.

**/
IniFormat ini_ntof (const IniFormatNum format_num) {

	#define __MAX_1_BITS__ 1
	#define __MAX_2_BITS__ 3
	#define __MAX_3_BITS__ 7
	#define __MAX_4_BITS__ 15
	#define __MAX_5_BITS__ 31
	#define __MAX_6_BITS__ 63
	#define __MAX_7_BITS__ 127
	#define __MAX_8_BITS__ 255
	#define __INIFORMAT_PROPERTIES__(NAME, OFFSET, SIZE, DEFVAL) \
		(unsigned char) ((format_num >> OFFSET) & __MAX_##SIZE##_BITS__),

	return (IniFormat) { INIFORMAT_TABLE_AS(__INIFORMAT_PROPERTIES__) };

	#undef __INIFORMAT_PROPERTIES__
	#undef __MAX_8_BITS__
	#undef __MAX_7_BITS__
	#undef __MAX_6_BITS__
	#undef __MAX_5_BITS__
	#undef __MAX_4_BITS__
	#undef __MAX_3_BITS__
	#undef __MAX_2_BITS__
	#undef __MAX_1_BITS__

}


													/** @utility{ini_get_bool} **/
/**

	@brief			Check whether a simple string matches one of the booleans listed
					in the private constant #INI_BOOLEANS (case-insensitive)
	@param			simple_string	A string to check (it can be `NULL`)
	@param			when_fail		A value that is returned if no matching boolean
									is found
	@return			The matching boolean (`0` or `1`) or @p when_fail if
					@p simple_string does not contain a valid INI boolean

	@include miscellanea/typed_ini.c

**/
int ini_get_bool (const char * const simple_string, const int when_fail) {

	if (!simple_string) {

		return when_fail;

	}

	register _LIBCONFINI_CHARBOOL_ bool_idx;
	register size_t pair_idx, chr_idx;

	for (pair_idx = 0; pair_idx < _LIBCONFINI_BOOLLEN_; pair_idx++) {

		bool_idx = 0;

		do {

			chr_idx = 0;

			while (
				_LIBCONFINI_CHR_CASEFOLD_(simple_string[chr_idx]) ==
				INI_BOOLEANS[pair_idx][bool_idx][chr_idx]
			) {

				if (!simple_string[chr_idx++]) {

					return (int) bool_idx;

				}

			}

			bool_idx ^= 1;

		} while (bool_idx);

	}

	return when_fail;

}


													/** @utility{ini_get_bool_i} **/
/**

	@brief			Check whether an INI string matches one of the booleans listed
					in the private constant #INI_BOOLEANS (case-insensitive)
	@param			ini_string		A string to check (it can be `NULL`)
	@param			when_fail		A value that is returned if no matching boolean
									is found
	@param			format			The format of the INI file
	@return			The matching boolean (`0` or `1`) or @p when_fail if
					@p ini_string does not contain a valid INI boolean

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	The @p format argument is used for the following fields:

	- `format.no_double_quotes`
	- `format.no_single_quotes`

	@include miscellanea/typed_ini.c

**/
int ini_get_bool_i (
	const char * const ini_string,
	const int when_fail,
	const IniFormat format
) {

	if (!ini_string) {

		return when_fail;

	}

	register size_t chr_idx_i;

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Format supports escape sequences (const)
		FLAG_8		Unescaped single quotes are odd right now
		FLAG_16		Unescaped double quotes are odd right now
		FLAG_32		This is an escaped single/double quote in a format that supports
					single/double quotes
		FLAG_64		This is a space
		FLAG_128	Skip this character

	*/

	register uint_least8_t
		abcd	=	INIFORMAT_HAS_NO_ESC(format) ?
						67
					:
						68 |
						(format.no_double_quotes << 1) |
						format.no_single_quotes;

	register _LIBCONFINI_CHARBOOL_ bool_idx;
	size_t pair_idx, chr_idx_s, nbacksl;

	for (pair_idx = 0; pair_idx < _LIBCONFINI_BOOLLEN_; pair_idx++) {

		bool_idx = 0;


		/* \                                /\
		\ */     pair_match:               /* \
		 \/     ______________________     \ */


		abcd = (abcd & 7) | 64;
		chr_idx_s = chr_idx_i = nbacksl = 0;

		do {

			if ((abcd & 4) && ini_string[chr_idx_i] == _LIBCONFINI_BACKSLASH_) {

				for (
					abcd &= 63, nbacksl++;
						ini_string[++chr_idx_i] == _LIBCONFINI_BACKSLASH_;
					nbacksl++
				);

			}

			/*  Keep this algorithm identical to #ini_string_match_si()  */

			abcd	=	!(abcd & 10) && ini_string[chr_idx_i] == _LIBCONFINI_DOUBLE_QUOTES_ ?
							(
								nbacksl & 1 ?
									(abcd & 63) | 32
								:
									((abcd & 223) | 128) ^ 16
							)
						: !(abcd & 17) && ini_string[chr_idx_i] == _LIBCONFINI_SINGLE_QUOTES_ ?
							(
								nbacksl & 1 ?
									(abcd & 63) | 32
								:
									((abcd & 223) | 128) ^ 8
							)
						: (abcd & 24) || !is_some_space(ini_string[chr_idx_i], _LIBCONFINI_WITH_EOL_) ?
							abcd & 31
						: abcd & 64 ?
							(abcd & 223) | 128
						:
							(abcd & 95) | 64;


			if (nbacksl) {

				nbacksl = (abcd & 32 ? nbacksl + 2 : nbacksl + 3) >> 1;

				while (--nbacksl) {

					if (
						INI_BOOLEANS[pair_idx][bool_idx][chr_idx_s++] !=
						_LIBCONFINI_BACKSLASH_
					) {

						goto next_bool;

					}

				}

			}

			if (
				(abcd & 128) || (
					(abcd & 64) && !INI_BOOLEANS[pair_idx][bool_idx][chr_idx_s]
				)
			) {

				continue;

			}

			if (
				abcd & 64 ?
					INI_BOOLEANS[pair_idx][bool_idx][chr_idx_s] != _LIBCONFINI_COLLAPSED_ ||
					!INI_BOOLEANS[pair_idx][bool_idx][chr_idx_s + 1]
				:
					_LIBCONFINI_CHR_CASEFOLD_(ini_string[chr_idx_i]) !=
					_LIBCONFINI_CHR_CASEFOLD_(INI_BOOLEANS[pair_idx][bool_idx][chr_idx_s])
			) {

				goto next_bool;

			}

			chr_idx_s++;

		} while (ini_string[chr_idx_i++]);

		return (int) bool_idx;


		/* \                                /\
		\ */     next_bool:                /* \
		 \/     ______________________     \ */


		if ((bool_idx ^= 1)) {

			goto pair_match;

		}

	}

	return when_fail;

}



		/*  LINKS - In case you don't have `#include <stdlib.h>` in your source  */


/**

	@alias{ini_get_int}
		Pointer to
		[`atoi()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi)
	@alias{ini_get_lint}
		Pointer to
		[`atol()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol)
	@alias{ini_get_llint}
		Pointer to
		[`atoll()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll)
	@alias{ini_get_double}
		Pointer to
		[`atof()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof)

**/

int (* const ini_get_int) (const char * ini_string) = atoi;

long int (* const ini_get_lint) (const char * ini_string) = atol;

long long int (* const ini_get_llint) (const char * ini_string) = atoll;

double (* const ini_get_double) (const char * ini_string) = atof;

/*  Legacy support -- please **do not use this**!  */
#ifdef ini_get_float
#undef ini_get_float
#endif

/**

	@brief			Legacy support for parsing a `double` data type -- please _do
					not use this function_: use `ini_get_double()` instead
	@deprecated		Deprecated since version 1.12.0 (it will be removed in version
					2.0.0) -- please use #ini_get_double() instead
	@param			ini_string

**/
double (* const ini_get_float) (const char * ini_string) = atof;



		/*  GLOBAL VARIABLES  */


bool INI_GLOBAL_LOWERCASE_MODE = _LIBCONFINI_FALSE_;

char * INI_GLOBAL_IMPLICIT_VALUE = (char *) 0;

size_t INI_GLOBAL_IMPLICIT_V_LEN = 0;



/** @endfnlist **/


/*  EOF  */

