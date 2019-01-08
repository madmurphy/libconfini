/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */

/**

	@file		confini.c
	@brief		libconfini functions
	@author		Stefano Gioffr&eacute;
	@copyright	GNU Public License v3
	@date		2016-2019
	@see		https://github.com/madmurphy/libconfini/

**/


           /*/|
          (_|_)      _ _ _                      __ _       _
                    | (_) |__   ___ ___  _ __  / _(_)_ __ (_)
                    | | | '_ \ / __/ _ \| '_ \| |_| | '_ \| |
                    | | | |_) | (_| (_) | | | |  _| | | | | |
                    |_|_|_.__/ \___\___/|_| |_|_| |_|_| |_|_|      _ _
                                                                  ( | )
                                                                  |/*/



#include <stdlib.h>
#include "confini.h"


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
					The format of the INI file from which @p ini_string has been extracted
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

	@property	IniFormat::delimiter_symbol
					The symbol to be used as delimiter (only ASCII allowed); if set
					to `0`, any space is delimiter
					(`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`); if, within the format
					given, this matches a metacharacter (`'\\'`, `'\''`, `'\"'`),
					its role as metacharacter will have higher priority than its
					role as delimiter symbol (i.e., the format will have no
					delimiter symbol); you may use `enum` #IniDelimiters for this.
	@property	IniFormat::case_sensitive
					If set to `true`, string comparisons will be always performed
					case-sensitive.
	@property	IniFormat::semicolon_marker
					The rule of the semicolon character (use `enum`
					#IniCommentMarker for this).
	@property	IniFormat::hash_marker
					The rule of the hash character (use `enum` #IniCommentMarker for
					this).
	@property	IniFormat::section_paths
					Defines whether and how the format supports sections (use `enum`
					#IniSectionPaths for this).
	@property	IniFormat::multiline_nodes
					Defines which class of entries are allowed to be multi-line (use
					`enum` #IniMultiline for this).
	@property	IniFormat::no_spaces_in_names
					If set to `true`, key and section names containing spaces (even
					within quotes) will be rendered as #INI_UNKNOWN. Note that
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
					always be dispatched using the values given by the global
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
					If set to `true`, allows what follows `/[#;]\s/` to be parsed as
					a disabled entry.
	@property	IniFormat::disabled_can_be_implicit
					If set to `true`, comments that do not contain a delimiter
					symbol will never be parsed as disabled keys, but always as
					simple comments.



	@struct		IniStatistics

	@property	IniStatistics::format
					The format of the INI file (see `struct` #IniFormat)
	@property	IniStatistics::bytes
					The size of the parsed file in bytes
	@property	IniStatistics::members
					The size of the parsed file in members (nodes) -- this number
					equals the number of dispatches



	@struct		IniDispatch

	@property	IniDispatch::format
					The format of the INI file (see `struct` #IniFormat)
	@property	IniDispatch::type
					The dispatch type (see `enum` #IniNodeType)
	@property	IniDispatch::data
					It can be the content of a comment, a section path or a key name
	@property	IniDispatch::value
					It can be the value of a key element, an empty string or it can
					point to the address pointed by the global variable
					#INI_GLOBAL_IMPLICIT_VALUE
	@property	IniDispatch::append_to
					The current section path
	@property	IniDispatch::d_len
					The length of the string #IniDispatch::data
	@property	IniDispatch::v_len
					The length of the string #IniDispatch::value
	@property	IniDispatch::at_len
					The length of the string #IniDispatch::append_to
	@property	IniDispatch::dispatch_id
					The dispatch id


**/



		/*\
		|*|
		|*|     LOCAL ENVIRONMENT
		|*|    ________________________________
		\*/



		/*  ALIASES  */


#define _LIBCONFINI_FALSE_ 0
#define _LIBCONFINI_TRUE_ 1
#define _LIBCONFINI_BOOL_ unsigned char
#define _LIBCONFINI_SIMPLE_SPACE_ 32
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



		/*  FUNCTIONAL MACROS AND CONSTANTS  */


/*  The character that will replace sequences of one or more spaces (`/\s+/`)  */
#define _LIBCONFINI_COLLAPSED_ _LIBCONFINI_SIMPLE_SPACE_

/*

	This may be any character, in theory... But after the left-trim of each node a
	leading space works pretty well as metacharacter...

*/
#define _LIBCONFINI_INLINE_MARKER_ _LIBCONFINI_SIMPLE_SPACE_

/*

	Checks whether a character can be escaped within a given format

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

	Checks whether a character marks a block containing a comment or a disabled
	entry within a given format

*/
#define _LIBCONFINI_IS_DIS_MARKER_(CHR, FMT) ( \
		CHR == _LIBCONFINI_HASH_ ? \
			FMT.hash_marker == INI_DISABLED_OR_COMMENT \
		: \
			CHR == _LIBCONFINI_SEMICOLON_ && FMT.semicolon_marker == INI_DISABLED_OR_COMMENT \
	)


/*

	Checks whether a character represents any marker within a given format

*/
#define _LIBCONFINI_IS_ANY_MARKER_(CHR, FMT) ( \
		CHR == _LIBCONFINI_HASH_ ? \
			FMT.hash_marker != INI_IS_NOT_A_MARKER \
		: \
			CHR == _LIBCONFINI_SEMICOLON_ && FMT.semicolon_marker != INI_IS_NOT_A_MARKER \
	)


/*

	Checks whether a character marks a block that must be ignored within a given
	format

*/
#define _LIBCONFINI_IS_IGN_MARKER_(CHR, FMT) ( \
		CHR == _LIBCONFINI_HASH_ ? \
			FMT.hash_marker == INI_IGNORE \
		: \
			CHR == _LIBCONFINI_SEMICOLON_ && FMT.semicolon_marker == INI_IGNORE \
	)


/*

	Maybe in the future there will be support for UTF-8 casefold, but for now only
	ASCII...

*/
#define _LIBCONFINI_CHR_CASEFOLD_(CHR) (CHR > 0x40 && CHR < 0x5b ? CHR | 0x60 : CHR)


/*

	Possible depths of `_LIBCONFINI_SPACES_` (see function #is_some_space()).

	Please, consider the following three constants as belonging together to a
	virtual opaque `enum`.

*/
#define _LIBCONFINI_WITH_EOL_ -1
#define _LIBCONFINI_NO_EOL_ 1
#define _LIBCONFINI_JUST_S_T_ 3


/*

	Other constants related to `_LIBCONFINI_SPACES_`

*/
#define _LIBCONFINI_EOL_IDX_ 0
#define _LIBCONFINI_SPALEN_ 6


/*  The list of space characters -- do not change its order!  */
static const char _LIBCONFINI_SPACES_[_LIBCONFINI_SPALEN_] = {
	_LIBCONFINI_LF_,
	_LIBCONFINI_CR_,
	_LIBCONFINI_VT_,
	_LIBCONFINI_FF_,
	_LIBCONFINI_HT_,
	_LIBCONFINI_SIMPLE_SPACE_
};


/**

	@brief	A list of possible string representations of boolean pairs

	There may be infinite pairs here. Each pair must be presented in this order:

	1. Signifier of `false`
	2. Signifier of `true`

	@note	Everything **must** be lowercase in this list.

**/
static const char * const INI_BOOLEANS[][2] = {
	{ "no", "yes" },
	{ "false", "true" },
	{ "off", "on" }
};



		/*  ABSTRACT UTILITIES  */


/**

	@brief			Checks whether a character is a space
	@param			chr				The target character
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			A boolean: `true` if the character matches, `false` otherwise

**/
static inline _LIBCONFINI_BOOL_ is_some_space (const char chr, const int8_t depth) {
	register int8_t idx = depth;
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
static inline size_t ltrim_s (const char * const str, const size_t offs, const int8_t depth) {
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
static inline size_t ltrim_h (char * const str, const size_t offs, const int8_t depth) {
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
static inline size_t ltrim_hh (char * const str, const size_t offs, const int8_t depth) {
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
static inline size_t rtrim_s (const char * const str, const size_t len, const int8_t depth) {
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
static inline size_t rtrim_h (char * const str, const size_t len, const int8_t depth) {
	register size_t idx = len;
	while (idx > 0 && is_some_space(str[idx - 1], depth)) { str[--idx] = '\0'; }
	return idx;
}


/**

	@brief			Unescaped soft right trim (right trim of `/(\s+|\\[\n\r])+$/`)
					-- does not change the buffer
	@param			str				The target string
	@param			len				The length of the string
	@return			The length of the string until the last non-space character

**/
static inline size_t urtrim_s (const char * const str, const size_t len) {

	register uint8_t abcd = 1;
	register size_t idx = len;


    /* \                                /\
    \ */     continue_trim:            /* \
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
			goto continue_trim;

		case _LIBCONFINI_LF_:
		case _LIBCONFINI_CR_:

			abcd = 3;
			goto continue_trim;

		case _LIBCONFINI_BACKSLASH_:

			if (abcd >>= 1) {

				goto continue_trim;

			}

	}

	return idx + 1;

}


/**

	@brief			Converts an ASCII string to lower case
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
					`/^(\s+|\\[\n\r]|''|"")+/`) -- **does** change the buffer
	@param			srcstr			The target string (it may contain multi-line
									escape sequences)
	@param			offs			The offset where to start the left trim
	@param			format			The format of the INI file
	@return			The offset of the first non-space character

**/
static inline size_t qultrim_h (char * const srcstr, const size_t offs, const IniFormat format) {

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		Erase previous character
		FLAG_64		Erase this character
		FLAG_128	Continue the loop

	*/

	register uint8_t abcd = (format.no_double_quotes ? 130 : 128) | format.no_single_quotes;
	size_t idx = offs;

	for (; abcd & 128; idx++) {

		abcd	= 	!(abcd & 28) && is_some_space(srcstr[idx], _LIBCONFINI_NO_EOL_) ?
						(abcd & 207) | 64
					: !(abcd & 12) && (srcstr[idx] == _LIBCONFINI_LF_ || srcstr[idx] == _LIBCONFINI_CR_) ?
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

	}

	return abcd & 28 ? idx - 2 : idx - 1;

}


/**

	@brief			Gets the position of the first occurence out of quotes of a
					given character, stopping after a given number of charcters
	@param			str				The string where to search
	@param			chr				The character to to search
	@param			len				The maximum number of characters to read
	@param			format			The format of the INI file
	@return			The offset of the first occurence of @p chr, or @p len if
					@p chr has not been not found

**/
static inline size_t getn_metachar_pos (const char * const str, const char chr, const size_t len, const IniFormat format) {

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

		register uint8_t abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

			idx < len && ((abcd & 12) || (chr ? str[idx] != chr : !is_some_space(str[idx], _LIBCONFINI_WITH_EOL_)));

		abcd	=	str[idx] == _LIBCONFINI_BACKSLASH_ ? abcd ^ 16
					: !(abcd & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abcd ^ 8
					: !(abcd & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abcd ^ 4
					: abcd & 15,
		idx++

	);

	return idx;

}


/**

	@brief			Gets the position of the first occurence out of quotes of a
					given character
	@param			str				The string where to search
	@param			chr				The character to to search
	@param			format			The format of the INI file
	@return			The offset of the first occurence of @p chr or the length of
					@p str if @p chr has not been not found

**/
static inline size_t get_metachar_pos (const char * const str, const char chr, const IniFormat format) {

	size_t idx = 0;

	/*

	Mask `abcd` (5 bits used):

		--> As in #getn_metachar_pos()

	*/

	for (

		register uint8_t abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

			str[idx] && ((abcd & 12) || (chr ? str[idx] != chr : !is_some_space(str[idx], _LIBCONFINI_NO_EOL_)));

		abcd	=	str[idx] == _LIBCONFINI_BACKSLASH_ ? abcd ^ 16
					: !(abcd & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abcd ^ 8
					: !(abcd & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abcd ^ 4
					: abcd & 15,
		idx++

	);

	return idx;

}


/**

	@brief			Replaces `/\\(\n\r?|\r\n?)[\t \v\f]*[#;]/` or `/\\(\n\r?|\r\n?)/`
					with `"$1"`
	@param			srcstr			The target string (it may contain multi-line
									escape sequences)
	@param			len				Length of the string
	@param			is_disabled		The string represents a disabled entry
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t unescape_cr_lf (char * const srcstr, const size_t len, const _LIBCONFINI_BOOL_ is_disabled, const IniFormat format) {

	register size_t idx_s = 0, idx_d = 0;
	register uint8_t eol_i = _LIBCONFINI_EOL_IDX_;
	register _LIBCONFINI_BOOL_ is_escaped = _LIBCONFINI_FALSE_;
	size_t probe;

	while (idx_s < len) {

		if (
			is_escaped && (
				srcstr[idx_s] == _LIBCONFINI_SPACES_[eol_i] || srcstr[idx_s] == _LIBCONFINI_SPACES_[eol_i ^= 1]
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

	@brief			Sanitizes a section path
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

	register uint16_t abcd = (format.no_double_quotes ? 1826 : 1824) | format.no_single_quotes;
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
							!(abcd & 4) ?
								(abcd & 3839) | 1540
							: abcd & 256 ?
								(abcd & 3867) | 3840
							:
								(abcd & 3579) | 1280
						)
					: !(abcd & 22) && secpath[idx_s] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							!(abcd & 8) ?
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

	register uint16_t abcd	=	(is_some_space(*ini_string, _LIBCONFINI_WITH_EOL_) ? 128 : 160) |
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
							!(abcd & 4) ?
								(abcd & 239) | 196
							: abcd & 64 ?
								(abcd & 299) | 256
							:
								(abcd & 171) | 160
						)
					: !(abcd & 22) && ini_string[idx_s] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							!(abcd & 8) ?
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

			ini_string[idx_d++] = abcd & 44 ? ini_string[idx_s] : _LIBCONFINI_COLLAPSED_;

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
					`ini_string.replace(/^[\n\r]\s*|\s+/g, " ")`
	@param			ini_string		The string to collapse -- multi-line escape
									sequences must be already unescaped at this
									stage
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t collapse_spaces (char * const ini_string, const IniFormat format) {

	/*

	Mask `abcd` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		This is a space out of quotes
		FLAG_64		Jump this character

	*/

	register uint8_t abcd = (format.no_double_quotes ? 34 : 32) | format.no_single_quotes;
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

	@brief			Out of quotes similar to ECMAScript
					`str.replace(/''|""/g, "")`
	@param			str				The string to collapse
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t collapse_empty_quotes (char * const str, const IniFormat format) {

	/*

	Mask `abcd` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		This is an opening single/double quote
		FLAG_64		These are empty quotes

	*/

	register uint8_t abcd = (format.no_double_quotes << 1) | format.no_single_quotes;
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

	@brief			Removes all comment initializers (`#` and/or `;`) from the
					beginning of each line of a comment
	@param			srcstr			The comment to parse (it may contain multi-line
									escape sequences)
	@param			len				The length of @p srcstr
	@param			format			The format of the INI file
	@return			The new length of the string

	- In multi-line comments: `srcstr.replace(/(^|\n\r?|\r\n?)[\t \v\f]*[#;]+/g, "$1")`
	- In single-line comments: `srcstr.replace(/^[\t \v\f]*[#;]+/, "")`

**/
static size_t uncomment (char * const srcstr, size_t len, const IniFormat format) {

	register size_t idx_s, idx_d = 0;

	if (format.multiline_nodes == INI_MULTILINE_EVERYWHERE) {

		/*

			The comment can be multi-line

		*/

		/*

		Mask `abcd` (6 bits used):

			FLAG_1		Don't erase any character
			FLAG_2		We are in an odd sequence of backslashes
			FLAG_4		This new line character is escaped
			FLAG_8		This character is a comment character and follows `/(\n\s*|\r\s*)/`
			FLAG_16		This character is a part of a group of spaces following
						a new line (`/(\n|\r)[\t \v\f]+/`)
			FLAG_32		This character is *not* a new line character (`/[^\r\n]/`)

		*/

		idx_s = 0;

		for (register uint8_t abcd = 0; idx_s < len; idx_s++) {

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

		/*

			The comment cannot be multi-line

		*/

		for (

			idx_s = ltrim_s(srcstr, 0, _LIBCONFINI_WITH_EOL_);

				idx_s < len && _LIBCONFINI_IS_ANY_MARKER_(srcstr[idx_s], format);

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

	@brief			Tries to determine the type of a member "as if it was active"
	@param			srcstr			String containing an individual node (it may
									contain multi-line escape sequences)
	@param			len				Length of the node
	@param			allow_implicit	A boolean: `true` if keys without a
									delimiter are allowed, `false` otherwise
	@param			format			The format of the INI file
	@return			The node type (see header)

**/
static uint8_t get_type_as_active (
	const char * const srcstr,
	const size_t len,
	const _LIBCONFINI_BOOL_ allow_implicit,
	const IniFormat format
) {

	const _LIBCONFINI_BOOL_ invalid_delimiter = _LIBCONFINI_IS_ESC_CHAR_(format.delimiter_symbol, format);

	if (
		!len || _LIBCONFINI_IS_ANY_MARKER_(*srcstr, format) || (
			*((unsigned char *) srcstr) == format.delimiter_symbol && !invalid_delimiter
		)
	) {

		return INI_UNKNOWN;

	}

	register uint16_t abcd;
	register size_t idx;

	if (format.section_paths != INI_NO_SECTIONS && *srcstr == _LIBCONFINI_OPEN_SECTION_) {

		if (format.no_spaces_in_names) {

			/*

				Search for the CLOSE_SECTION character and possible spaces in names
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

			for (

				idx = 1,

				abcd	=	(format.section_paths == INI_ONE_LEVEL_ONLY ? 772: 768) |
							(format.no_double_quotes << 1) |
							format.no_single_quotes;

					abcd & 256;

				idx++

			) {

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
									!(abcd & 32) ?
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
									!(abcd & 32) ?
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

			}

			if (abcd & 512) {

				return INI_UNKNOWN;

			}

		} else if ((idx = getn_metachar_pos(srcstr, _LIBCONFINI_CLOSE_SECTION_, len, format) + 1) > len) {

			return INI_UNKNOWN;

		}

		/*

			Scan for possible non-space characters following the CLOSE_SECTION
			character: if found the node cannot represent a section path (but it can
			possibly represent a key).

		*/

		/*

		Recycling variable `abcd`... Mask `abcd` (3 bits used):

			FLAG_1		Last character was a backslash
			FLAG_2		Throw `return INI_SECTION;`
			FLAG_4		Continue the loop

		*/

		abcd = idx < len ? 4 : 2;


		/* \                                /\
		\ */     nonspace_check:           /* \
		 \/     ______________________     \ */


		if (abcd & 2) {

			return INI_SECTION;

		}

		if (abcd & 4) {

			switch (srcstr[idx++]) {

				case _LIBCONFINI_VT_:
				case _LIBCONFINI_FF_:
				case _LIBCONFINI_HT_:
				case _LIBCONFINI_SIMPLE_SPACE_:

					abcd = abcd & 1 ? 0 : idx < len ? 4 : 2;
					goto nonspace_check;

				case _LIBCONFINI_LF_:
				case _LIBCONFINI_CR_:

					abcd = idx < len ? 4 : 2;
					goto nonspace_check;

				case _LIBCONFINI_BACKSLASH_:

					abcd = abcd & 1 ? 1 : idx < len ? 5 : 1;
					goto nonspace_check;

			}

		}

	}

	/*

		It can be just a key...

	*/

	if (invalid_delimiter && !allow_implicit) {

		return INI_UNKNOWN;

	}

	/*

	Recycling variable `abcd`... Mask `abcd` (2 bits used):

		FLAG_1		The delimiter **must** be present
		FLAG_2		Search for spaces in names

	*/

	abcd = (format.no_spaces_in_names << 1) | (allow_implicit ? 0 : 1);

	if (abcd) {

		idx = getn_metachar_pos(srcstr, (char) format.delimiter_symbol, len, format);

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

	@brief			Examines a (single-/multi- line) segment and checks whether
					it contains sub-segments
	@param			srcstr			Segment to examine (it may contain multi-line
									escape sequences)
	@param			format			The format of the INI file
	@return			Number of segments found

**/
static size_t further_cuts (char * const srcstr, const IniFormat format) {

	_LIBCONFINI_BOOL_ forget_me;
	register uint8_t abcd = (format.no_double_quotes << 1) | format.no_single_quotes;
	register size_t idx;
	size_t focus_at, unparsable_at, search_at = 0, segm_size = 0;


    /* \                                /\
    \ */     search_for_cuts:          /* \
     \/     ______________________     \ */


	if (!srcstr[search_at]) {

		goto no_more_cuts;

	}

	unparsable_at = 0;
	forget_me = _LIBCONFINI_IS_IGN_MARKER_(srcstr[search_at], format);

	if (!forget_me) {

		segm_size++;

	}

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		The previous character was not a space
		FLAG_64		This can be a disabled entry
		FLAG_128	This is nothing but a (multi-line) comment

		Note:	For FLAG_1 and FLAG_2 I keep the values already assigned at the
				beginning of the function; all other flags will be set to zero.

	*/

	abcd	=	_LIBCONFINI_IS_DIS_MARKER_(srcstr[search_at], format) && (
					format.disabled_after_space || !is_some_space(srcstr[search_at + 1], _LIBCONFINI_NO_EOL_)
				) ?
					(abcd & 3) | 64
				: format.multiline_nodes == INI_MULTILINE_EVERYWHERE && (
					srcstr[search_at] == _LIBCONFINI_INLINE_MARKER_ || _LIBCONFINI_IS_ANY_MARKER_(srcstr[search_at], format)
				) ?
					(abcd & 3) | 128
				:
					abcd & 3;


	if (abcd & 192) {

		/*

			Node starts with `/[;#]/` and can be multi-line or represent a disabled
			entry

		*/

		for (idx = ltrim_s(srcstr, search_at + 1, _LIBCONFINI_NO_EOL_); srcstr[idx]; idx++) {

			if (_LIBCONFINI_IS_ANY_MARKER_(srcstr[idx], format)) {

				/*

					Search for inline comments in (supposedly) disabled items

				*/

				if (!(abcd & 172)) {

					focus_at = ltrim_s(srcstr, search_at + 1, _LIBCONFINI_NO_EOL_);

					if (get_type_as_active(srcstr + focus_at, idx - focus_at, format.disabled_can_be_implicit, format)) {

						if (!_LIBCONFINI_IS_IGN_MARKER_(srcstr[idx], format)) {

							srcstr[idx] = _LIBCONFINI_INLINE_MARKER_;
							segm_size++;

						}

						srcstr[idx - 1] = '\0';

						if (format.multiline_nodes != INI_MULTILINE_EVERYWHERE) {

							unparsable_at = idx + 1;
							break;

						}

					} else if (format.multiline_nodes != INI_MULTILINE_EVERYWHERE) {

						unparsable_at = search_at + 1;
						break;

					}

				}

				abcd = (abcd & 239) | 32;

			} else if (srcstr[idx] == _LIBCONFINI_LF_ || srcstr[idx] == _LIBCONFINI_CR_) {

				/*

					Search for `/\\(?:\n\r?|\r\n?)\s*[^;#]/` in multi-line disabled items

				*/

				focus_at = ltrim_s(srcstr, search_at + 1, _LIBCONFINI_NO_EOL_);
				idx = ltrim_s(srcstr, idx + 1, _LIBCONFINI_WITH_EOL_);

				if (
					forget_me ?
						!_LIBCONFINI_IS_IGN_MARKER_(srcstr[idx], format)
					:
						_LIBCONFINI_IS_IGN_MARKER_(srcstr[idx], format) || !(
							(
								format.multiline_nodes < 2 && _LIBCONFINI_IS_DIS_MARKER_(srcstr[idx], format) && (
									format.disabled_after_space || !(abcd & 64) || !is_some_space(srcstr[idx + 1], _LIBCONFINI_NO_EOL_)
								)
							) || (
								format.multiline_nodes == INI_MULTILINE_EVERYWHERE && _LIBCONFINI_IS_ANY_MARKER_(srcstr[idx], format) && !(
									(abcd & 64) && get_type_as_active(
										srcstr + focus_at,
										idx - focus_at,
										format.disabled_can_be_implicit,
										format
									)
								)
							)
						)
				) {

					rtrim_h(srcstr, idx, _LIBCONFINI_WITH_EOL_);
					search_at = qultrim_h(srcstr, idx, format);
					goto search_for_cuts;

				}

				abcd &= 207;

			} else if (is_some_space(srcstr[idx], _LIBCONFINI_NO_EOL_)) {

				abcd &= 207;

			} else {

				abcd	=	srcstr[idx] == _LIBCONFINI_BACKSLASH_ ?
								(abcd | 32) ^ 16
							: !(abcd & 22) && srcstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								(abcd | 32) ^ 8
							: !(abcd & 25) && srcstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								(abcd | 32) ^ 4
							:
								(abcd & 239) | 32;

			}

		}

		if (format.multiline_nodes != INI_MULTILINE_EVERYWHERE && !unparsable_at) {

			focus_at = ltrim_s(srcstr, search_at + 1, _LIBCONFINI_NO_EOL_);

			if (srcstr[focus_at] && !get_type_as_active(srcstr + focus_at, idx - focus_at, format.disabled_can_be_implicit, format)) {

				unparsable_at = search_at + 1;

			}

		}

	} else if (_LIBCONFINI_IS_ANY_MARKER_(srcstr[search_at], format)) {

		/*

			Node starts with `/[;#]/` but cannot be multi-line nor represent a
			disabled entry

		*/

		unparsable_at = search_at + 1;

	} else {

		/*

			Node is active: search for inline comments

		*/

		/*

		Recycling variable `abcd`... Mask `abcd` (7 bits used):

			FLAG_1		Single quotes are not metacharacters (const)
			FLAG_2		Double quotes are not metacharacters (const)
			FLAG_4		Unescaped single quotes are odd right now
			FLAG_8		Unescaped double quotes are odd right now
			FLAG_16		We are in an odd sequence of backslashes
			FLAG_32		This is neither a hash nor a semicolon character
			FLAG_64		The previous character was not a space

		Note:	For FLAG_1 and FLAG_2 I keep the values already assigned at the
				beginning of the function; all other flags have been already set to
				zero (see previous usage of `abcd` within this function).

		*/

		for (abcd |= 96, idx = search_at + 1; srcstr[idx]; idx++) {

			abcd	=	_LIBCONFINI_IS_ANY_MARKER_(srcstr[idx], format) ?
							abcd & 79
						: is_some_space(srcstr[idx], _LIBCONFINI_WITH_EOL_) ?
							(abcd & 47) | 32
						: srcstr[idx] == _LIBCONFINI_BACKSLASH_ ?
							(abcd | 96) ^ 16
						: !(abcd & 22) && srcstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
							(abcd | 96) ^ 8
						: !(abcd & 25) && srcstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
							(abcd | 96) ^ 4
						:
							(abcd & 111) | 96;


			if (!(abcd & 124)) {

				srcstr[idx - 1] = '\0';

				if (!_LIBCONFINI_IS_IGN_MARKER_(srcstr[idx], format)) {

					srcstr[idx] = _LIBCONFINI_INLINE_MARKER_;

					if (format.multiline_nodes != INI_MULTILINE_EVERYWHERE) {

						segm_size++;

					}

				}

				if (format.multiline_nodes != INI_MULTILINE_EVERYWHERE) {

					unparsable_at = idx + 1;
					break;

				}

				search_at = idx;
				goto search_for_cuts;

			}

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


    /* \                                /\
    \ */     no_more_cuts:             /* \
     \/     ______________________     \ */


	return segm_size;

}

/**  @startfnlist  **/



		/*\
		|*|
		|*|     GLOBAL ENVIRONMENT
		|*|    ________________________________
		\*/



		/*  LIBRARY'S MAIN FUNCTIONS  */


												/**  @utility{load_ini_file}  **/
/**

	@brief			Parses an INI file and dispatches its content using a `FILE`
					structure as argument
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

	The @p ini_file parameter must be a `FILE` handle with read privileges. In some
	platforms, such as Microsoft Windows, it might be needed to add the binary
	specifier to the mode string (`"b"`) in order to prevent discrepancies between
	the physical size of the file and its computed size:

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	FILE * my_file = fopen("example.conf", "rb");
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	The parsing algorithms used by **libconfini** are able to parse any type of file
	encoded in 8-bit code units, as long as the characters that match the regular
	expression `/[\s\[\]\.\\;#"']/` represent the same code points they represent in
	ASCII, independently of platform-specific conventions (see, for example, UTF-8
	and ISO-8859-1).

	@note	In order to be null byte injection safe, `NUL` characters, if present in
			the file, will be removed from the dispatched strings.

	The function @p f_init (see #IniStatsHandler data type) will be invoked with two
	arguments: `statistics` (a pointer to an #IniStatistics structure containing
	some properties about the file read) and `user_data` (the custom argument
	@p user_data previously passed). If @p f_init returns a non-zero value the
	caller function will be interrupted.

	The function @p f_foreach (see #IniDispHandler data type) will be invoked with
	two arguments: `dispatch` (a pointer to an #IniDispatch structure containing the
	parsed member of the INI file) and `user_data` (the custom argument @p user_data
	previously passed). If @p f_foreach returns a non-zero value the caller function
	will be interrupted.

	@include topics/load_ini_file.c

**/
int load_ini_file (
	FILE * const ini_file,
	const IniFormat format,
	const IniStatsHandler f_init,
	const IniDispHandler f_foreach,
	void * const user_data
) {

	fseek(ini_file, 0, SEEK_END);

	size_t tmp_uint_1 = ftell(ini_file);

	#define __N_BYTES__ tmp_uint_1

	char * const cache = (char *) malloc(__N_BYTES__ + 1);

	if (!cache) {

		return CONFINI_ENOMEM;

	}

	int return_value = CONFINI_SUCCESS;

	rewind(ini_file);

	if (fread(cache, 1, __N_BYTES__, ini_file) < __N_BYTES__) {

		return_value = CONFINI_EIO;
		goto free_and_exit;

	}

	cache[__N_BYTES__] = '\0';

	const _LIBCONFINI_BOOL_ valid_delimiter = !_LIBCONFINI_IS_ESC_CHAR_(format.delimiter_symbol, format);
	_LIBCONFINI_BOOL_ tmp_bool;
	register uint16_t abcd;
	register size_t idx;
	size_t tmp_uint_2, tmp_uint_3, node_at;

	/*

		PART ONE: Examine and isolate each segment

	*/

	#define __ISNT_ESCAPED__ tmp_bool
	#define __EOL_ID__ abcd
	#define __N_MEMBERS__ tmp_uint_2
	#define __LSHIFT__ tmp_uint_3

	/*  UTF-8 BOM  */
	__LSHIFT__ = *((unsigned char *) cache) == 0xEF && *((unsigned char *) cache + 1) == 0xBB && *((unsigned char *) cache + 2) == 0xBF ? 3 : 0;

	for (

		__N_MEMBERS__ = 0,
		__EOL_ID__ = _LIBCONFINI_EOL_IDX_,
		__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_,
		node_at = 0,
		idx = __LSHIFT__;

			idx < __N_BYTES__;

		idx++

	) {

		cache[idx - __LSHIFT__] = cache[idx];

		if (cache[idx] == _LIBCONFINI_SPACES_[__EOL_ID__] || cache[idx] == _LIBCONFINI_SPACES_[__EOL_ID__ ^= 1]) {

			if (format.multiline_nodes == INI_NO_MULTILINE || __ISNT_ESCAPED__) {

				cache[idx - __LSHIFT__] = '\0';
				__N_MEMBERS__ += further_cuts(cache + qultrim_h(cache, node_at, format), format);
				node_at = idx - __LSHIFT__ + 1;

			} else if (cache[idx + 1] == _LIBCONFINI_SPACES_[__EOL_ID__ ^ 1]) {

				idx++;

			}

			__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_;

		} else if (cache[idx] == _LIBCONFINI_BACKSLASH_) {

			__ISNT_ESCAPED__ = !__ISNT_ESCAPED__;

		} else if (cache[idx]) {

			__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_;

		} else {

			/*  Remove `NUL` characters in the buffer (if any)  */
			__LSHIFT__++;

		}

	}

	const size_t len = idx - __LSHIFT__;

	while (idx > len) {

		cache[--idx] = '\0';

	}

	__N_MEMBERS__ += further_cuts(cache + qultrim_h(cache, node_at, format), format);

	/*  Debug  */

	/*

	for (size_t tmp = 0; tmp < len + 1; tmp++) {
		putchar(cache[tmp] == 0 ? '$' : cache[tmp]);
	}
	putchar('\n');

	*/

	IniStatistics this_doc = {
		.format = format,
		.bytes = __N_BYTES__,
		.members = __N_MEMBERS__
	};

	if (f_init && f_init(&this_doc, user_data)) {

		return_value = CONFINI_IINTR;
		goto free_and_exit;

	}

	#undef __LSHIFT__
	#undef __N_MEMBERS__
	#undef __EOL_ID__
	#undef __ISNT_ESCAPED__
	#undef __N_BYTES__

	/*

		PART TWO: Dispatch the parsed input

	*/

	if (!f_foreach) {

		goto free_and_exit;

	}

	#define __PARENT_IS_DISABLED__ tmp_bool
	#define __REAL_PARENT_LEN__ tmp_uint_1
	#define __CURR_PARENT_LEN__ tmp_uint_2
	#define __ITER__ tmp_uint_3

	__REAL_PARENT_LEN__ = 0, __CURR_PARENT_LEN__ = 0;

	size_t parse_at;
	char * curr_parent_str = cache + len, * subparent_str = curr_parent_str, * real_parent_str = curr_parent_str;

	IniDispatch dsp = {
		.format = format,
		.dispatch_id = 0
	};

	__PARENT_IS_DISABLED__ = _LIBCONFINI_FALSE_;

	for (node_at = 0, idx = 0; idx <= len; idx++) {

		if (cache[idx]) {

			continue;

		}

		if (!cache[node_at] || _LIBCONFINI_IS_IGN_MARKER_(cache[node_at], format)) {

			node_at = idx + 1;
			continue;

		}

		if (dsp.dispatch_id >= this_doc.members) {

			return_value = CONFINI_EOOR;
			goto free_and_exit;

		}

		dsp.data = cache + node_at;
		dsp.value = cache + idx;
		dsp.d_len = idx - node_at;
		dsp.v_len = 0;

		if (_LIBCONFINI_IS_DIS_MARKER_(*dsp.data, format)) {

			parse_at = ltrim_s(cache, node_at + 1, _LIBCONFINI_NO_EOL_);

			/*

				Search for inline comments left unmarked *inside* a parsable
				comment: if found, it means that the comment is not parsable.

			*/

			/*

			Mask `abcd` (10 bits used):

				FLAG_1		Single quotes are not metacharacters (const)
				FLAG_2		Double quotes are not metacharacters (const)
				FLAG_4		Allow disabled after space (const)
				FLAG_8		Unescaped single quotes are odd right now
				FLAG_16		Unescaped double quotes are odd right now
				FLAG_32		We are in an odd sequence of backslashes
				FLAG_64		We are in `\n[\t \v\f]*`
				FLAG_128	Last was not `[\t \v\f]` OR `FLAG_64 == FALSE`
				FLAG_256	Last was not `\s[;#]`, or it was but was semantic rather
							than syntactic
				FLAG_512	Continue the loop

			*/

			abcd	=	(format.disabled_after_space ? 516 : 512) |
						(format.no_double_quotes << 1) |
						format.no_single_quotes;


			for (__ITER__ = 1; (abcd & 512) && __ITER__ < dsp.d_len; __ITER__++) {

				abcd	=	dsp.data[__ITER__] == _LIBCONFINI_LF_ || dsp.data[__ITER__] == _LIBCONFINI_CR_ ?
								abcd | 448
							: _LIBCONFINI_IS_ANY_MARKER_(dsp.data[__ITER__], format) ?
								(
									(abcd & 64) && !_LIBCONFINI_IS_DIS_MARKER_(dsp.data[__ITER__], format) ?
										abcd & 31
									: abcd & 128 ?
										abcd & 543
									: abcd & 24 ?
										(abcd & 799) | 256
									:
										abcd & 287
								)
							: is_some_space(dsp.data[__ITER__], _LIBCONFINI_NO_EOL_) ?
								(
									abcd & 64 ?
										(abcd & 991) | 448
									: abcd & 260 ?
										(abcd & 799) | 256
									:
										abcd & 31
								)
							: dsp.data[__ITER__] == _LIBCONFINI_BACKSLASH_ ?
								((abcd & 831) | 256) ^ 32
							: !(abcd & 42) && dsp.data[__ITER__] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abcd & 799) | 256) ^ 16
							: !(abcd & 49) && dsp.data[__ITER__] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abcd & 799) | 256) ^ 8
							:
								(abcd & 799) | 256;

			}

			dsp.type		=	__ITER__ == dsp.d_len ?
										get_type_as_active(cache + parse_at, idx - parse_at, format.disabled_can_be_implicit, format)
									:
										INI_UNKNOWN;


			if (dsp.type) {

				dsp.data = cache + parse_at;
				dsp.d_len = idx - parse_at;

			}

			dsp.type |= 4;

		} else {

			dsp.type	=	_LIBCONFINI_IS_ANY_MARKER_(*dsp.data, format) ?
									INI_COMMENT
								: *dsp.data == _LIBCONFINI_INLINE_MARKER_ ?
									INI_INLINE_COMMENT
								:
									get_type_as_active(dsp.data, dsp.d_len, _LIBCONFINI_TRUE_, format);

		}

		if (__CURR_PARENT_LEN__ && *subparent_str) {

			__ITER__ = 0;

			do {

				curr_parent_str[__ITER__ + __CURR_PARENT_LEN__] = subparent_str[__ITER__];

			} while (subparent_str[__ITER__++]);

			__CURR_PARENT_LEN__ += __ITER__ - 1;
			subparent_str = curr_parent_str + __CURR_PARENT_LEN__;

		}

		if (__PARENT_IS_DISABLED__ && !(dsp.type & 4)) {

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

		dsp.d_len		=	dsp.type == INI_COMMENT ?
									uncomment(dsp.data, idx - node_at, format)
								: dsp.type == INI_INLINE_COMMENT ?
									uncomment(++dsp.data, idx - node_at - 1, format)
								: format.multiline_nodes != INI_NO_MULTILINE ?
									unescape_cr_lf(dsp.data, idx - node_at, dsp.type & 4, format)
								:
									idx - node_at;


		switch (dsp.type) {

			/*

			case INI_UNKNOWN:

				// Do nothing

				break;

			*/

			case INI_SECTION:
			case INI_DISABLED_SECTION:

				*dsp.data++ = '\0';
				__ITER__ = getn_metachar_pos(dsp.data, _LIBCONFINI_CLOSE_SECTION_, dsp.d_len, format);

				while (dsp.data[__ITER__]) {

					dsp.data[__ITER__++] = '\0';

				}

				dsp.d_len		=	format.section_paths == INI_ONE_LEVEL_ONLY ?
											collapse_everything(dsp.data, format)
										:
											sanitize_section_path(dsp.data, format);


				if (format.section_paths == INI_ONE_LEVEL_ONLY || *dsp.data != _LIBCONFINI_SUBSECTION_) {

					/*

						Append to root (it is an absolute path)

					*/

					curr_parent_str = dsp.data;
					__CURR_PARENT_LEN__ = dsp.d_len;
					subparent_str = cache + idx;
					dsp.append_to = subparent_str;
					dsp.at_len = 0;

				} else if (format.section_paths == INI_ABSOLUTE_ONLY || !__CURR_PARENT_LEN__) {

					/*

						Append to root and remove the leading dot (parent is root or
						relative paths are not allowed)

					*/

					curr_parent_str = ++dsp.data;
					__CURR_PARENT_LEN__ = --dsp.d_len;
					subparent_str = cache + idx;
					dsp.append_to = subparent_str;
					dsp.at_len = 0;

				} else if (dsp.d_len != 1) {

					/*

						Append to the current parent (it is a relative path and
						parent is not root)

					*/

					subparent_str = dsp.data;

				}

				if (INI_GLOBAL_LOWERCASE_MODE && !format.case_sensitive) {

					string_tolower(dsp.data);

				}

				break;

			case INI_KEY:
			case INI_DISABLED_KEY:

				if (
					valid_delimiter && dsp.d_len && (
						__ITER__ = getn_metachar_pos(dsp.data, (char) dsp.format.delimiter_symbol, dsp.d_len, format)
					) < dsp.d_len
				) {

					dsp.data[__ITER__] = '\0';
					dsp.value = dsp.data + __ITER__ + 1;

					switch ((format.preserve_empty_quotes << 1) | format.do_not_collapse_values) {

						case 0:	dsp.v_len = collapse_everything(dsp.value, format); break;

						case 1:	dsp.v_len = collapse_empty_quotes(dsp.value, format); break;

						case 2:	dsp.v_len = collapse_spaces(dsp.value, format); break;

						case 4:

							dsp.value += ltrim_h(dsp.value, 0, _LIBCONFINI_WITH_EOL_);
							dsp.v_len = rtrim_h(dsp.value, dsp.d_len + dsp.data - dsp.value, _LIBCONFINI_WITH_EOL_);
							break;

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

				dsp.append_to = cache + idx;
				dsp.at_len = 0;

		}

		if (f_foreach(&dsp, user_data)) {

			return_value = CONFINI_FEINTR;
			goto free_and_exit;

		}

		dsp.dispatch_id++;
		node_at = idx + 1;

	}

	#undef __ITER__
	#undef __CURR_PARENT_LEN__
	#undef __REAL_PARENT_LEN__
	#undef __PARENT_IS_DISABLED__


    /* \                                /\
    \ */     free_and_exit:            /* \
     \/     ______________________     \ */


	free(cache);

	return return_value;

}


												/**  @utility{load_ini_path}  **/
/**

	@brief			Parses an INI file and dispatches its content using a path as
					argument
	@param			path			The path of the INI file
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the
									first dispatch, or `NULL`
	@param			f_foreach		The function that will be invoked for each
									dispatch, or `NULL`
	@param			user_data		A custom argument, or `NULL`
	@return			Zero for success, otherwise an error code (see `enum`
					#ConfiniInterruptNo)

	The parsing algorithms used by **libconfini** are able to parse any type of file
	encoded in 8-bit code units, as long as the characters that match the regular
	expression `/[\s\[\]\.\\;#"']/` represent the same code points they represent in
	ASCII, independently of platform-specific conventions (see, for example, UTF-8
	and ISO-8859-1).

	@note	In order to be null byte injection safe, `NUL` characters, if present in
			the file, will be removed from the dispatched strings.

	For the two parameters @p f_init and @p f_foreach see function #load_ini_file().

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

	int return_value = load_ini_file(ini_file, format, f_init, f_foreach, user_data);

	fclose(ini_file);

	return return_value;

}



		/*  OTHER UTILITIES (NOT REQUIRED BY LIBCONFINI'S MAIN FUNCTIONS)  */


											/**  @utility{ini_string_match_ss}  **/
/**

	@brief			Compares two simple strings and checks if they match
	@param			simple_string_a		The first simple string
	@param			simple_string_b		The second simple string
	@param			format				The format of the INI file
	@return			A boolean: `true` if the two strings match, `false` otherwise

	Simple strings are user-given strings or the result of #ini_string_parse(). The
	@p format argument is used for the following fields:

	- `format.case_sensitive`

**/
_Bool ini_string_match_ss (const char * const simple_string_a, const char * const simple_string_b, const IniFormat format) {

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

		if (_LIBCONFINI_CHR_CASEFOLD_(simple_string_a[idx]) != _LIBCONFINI_CHR_CASEFOLD_(simple_string_b[idx])) {

			return _LIBCONFINI_FALSE_;

		}

	} while (simple_string_a[idx++]);

	return _LIBCONFINI_TRUE_;

}


											/**  @utility{ini_string_match_si}  **/
/**

	@brief			Compares a simple string and an INI string and and checks if
					they match
	@param			ini_string		The INI string escaped according to
									@p format
	@param			simple_string	The simple string
	@param			format			The format of the INI file
	@return			A boolean: `true` if the two strings match, `false` otherwise

	INI strings are the strings typically dispatched by #load_ini_file() and
	#load_ini_path(), which may contain quotes and the three escape sequences `\\`,
	`\'` and `\"`. Simple strings are user-given strings or the result of
	#ini_string_parse().

	In order to be suitable for both names and values, **this function always
	considers sequences of one or more spaces out of quotes in the INI string as
	collapsed**, even when `format.do_not_collapse_values` is set to `true`.

	This function grants that the result of the comparison between a simple string
	and an INI string

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	printf(
		"%s\n",
		ini_string_match_si(my_simple_string, my_ini_string, format) ?
			"They match"
		:
			"They don't match"
	);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	will always match the result of the _literal_ comparison between the simple
	string and the INI string after the latter has been parsed by
	#ini_string_parse() when `format.do_not_collapse_values` is set to `false`.

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	ini_string_parse(my_ini_string, format);

	printf(
		"%s\n",
		ini_string_match_ss(my_simple_string, my_ini_string, format) ?
			"They match"
		:
			"They don't match"
	);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	The @p format argument is used for the following fields:

	- `format.case_sensitive`
	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes`

	@include topics/ini_string_match_si.c

**/
_Bool ini_string_match_si (const char * const simple_string, const char * const ini_string, const IniFormat format) {

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

	register uint8_t abcd	=	INIFORMAT_HAS_NO_ESC(format) ?
									67
								:
									68 | (format.no_double_quotes << 1) | format.no_single_quotes;

	register size_t idx_i = 0;
	size_t idx_s = 0, nbacksl = 0;


    /* \                                /\
    \ */     si_match:                 /* \
     \/     ______________________     \ */


	if ((abcd & 4) && ini_string[idx_i] == _LIBCONFINI_BACKSLASH_) {

		for (abcd &= 63, nbacksl++; ini_string[++idx_i] == _LIBCONFINI_BACKSLASH_; nbacksl++);

	}

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

		nbacksl = ((abcd & 32 ? nbacksl : nbacksl + 1) >> 1) + 1;

		while (--nbacksl) {

			if (simple_string[idx_s++] != _LIBCONFINI_BACKSLASH_) {

				return _LIBCONFINI_FALSE_;

			}

		}

	}

	if ((abcd & 128) || ((abcd & 64) && !simple_string[idx_s])) {

		idx_i++;
		goto si_match;

	}

	if (
		abcd & 64 ?
			simple_string[idx_s] != _LIBCONFINI_COLLAPSED_ || !simple_string[idx_s + 1]
		: format.case_sensitive ?
			ini_string[idx_i] != simple_string[idx_s]
		:
			_LIBCONFINI_CHR_CASEFOLD_(ini_string[idx_i]) != _LIBCONFINI_CHR_CASEFOLD_(simple_string[idx_s])
	) {

		return _LIBCONFINI_FALSE_;

	}

	idx_s++;

	if (ini_string[idx_i++]) {

		goto si_match;

	}

	return _LIBCONFINI_TRUE_;

}


											/**  @utility{ini_string_match_ii}  **/
/**

	@brief			Compares two INI strings and checks if they match
	@param			ini_string_a	The first INI string unescaped according to
									@p format
	@param			ini_string_b	The second INI string unescaped according to
									@p format
	@param			format			The format of the INI file
	@return			A boolean: `true` if the two strings match, `false` otherwise

	INI strings are the strings typically dispatched by #load_ini_file() and
	#load_ini_path(), which may contain quotes and the three escape sequences `\\`,
	`\'` and `\"`.

	In order to be suitable for both names and values, **this function always
	considers sequences of one or more spaces out of quotes in both strings as
	collapsed**, even when `format.do_not_collapse_values` is set to `true`.

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

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	ini_string_parse(my_ini_string_1, format);
	ini_string_parse(my_ini_string_2, format);

	printf("%s\n",
		ini_string_match_ss(my_ini_string_1, my_ini_string_2, format) ?
			"They match"
		:
			"They don't match"
	);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	The @p format argument is used for the following fields:

	- `format.case_sensitive`
	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes`

**/
_Bool ini_string_match_ii (const char * const ini_string_a, const char * const ini_string_b, const IniFormat format) {

	const _LIBCONFINI_BOOL_ has_escape = !INIFORMAT_HAS_NO_ESC(format);
	register uint8_t side = 1;
	register _LIBCONFINI_BOOL_ turn_allowed = _LIBCONFINI_TRUE_;
	uint8_t abcd_pair[2];
	const char * chrptr_pair[2] = { ini_string_a, ini_string_b };
	size_t nbacksl_pair[2];

	/*

	Masks `abcd_pair[0]` and  `abcd_pair[1]` (7 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes and format supports
					escape sequences
		FLAG_32		This is a space
		FLAG_64		Skip this character

	*/

	abcd_pair[1] = *abcd_pair = 32 | (format.no_double_quotes << 1) | format.no_single_quotes;


    /* \                                /\
    \ */     ii_match:                 /* \
     \/     ______________________     \ */


	nbacksl_pair[side] = 0;

	if (has_escape && *chrptr_pair[side] == _LIBCONFINI_BACKSLASH_) {

		for (nbacksl_pair[side]++; *(++chrptr_pair[side]) == _LIBCONFINI_BACKSLASH_; nbacksl_pair[side]++);

		abcd_pair[side] = nbacksl_pair[side] & 1 ? (abcd_pair[side] & 31) | 16 : abcd_pair[side] & 15;

		if (
			(
				(abcd_pair[side] & 9) || *chrptr_pair[side] != _LIBCONFINI_SINGLE_QUOTES_
			) && (
				(abcd_pair[side] & 6) || *chrptr_pair[side] != _LIBCONFINI_DOUBLE_QUOTES_
			)
		) {

			nbacksl_pair[side]++;

		}

	} else {

		abcd_pair[side]	=	!(abcd_pair[side] & 25) && *chrptr_pair[side] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abcd_pair[side] & 111) | 64) ^ 4
							: !(abcd_pair[side] & 22) && *chrptr_pair[side] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abcd_pair[side] & 111) | 64) ^ 8
							: !(abcd_pair[side] & 12) && is_some_space(*chrptr_pair[side], _LIBCONFINI_WITH_EOL_) ?
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

	if (*nbacksl_pair || nbacksl_pair[1]) {

		if (*nbacksl_pair >> 1 != nbacksl_pair[1] >> 1) {

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
			**chrptr_pair != *chrptr_pair[1]
		:
			_LIBCONFINI_CHR_CASEFOLD_(**chrptr_pair) != _LIBCONFINI_CHR_CASEFOLD_(*chrptr_pair[1])
	) {

		return _LIBCONFINI_FALSE_;

	}

	if (**chrptr_pair) {

		(*chrptr_pair)++;

	}

	if (*chrptr_pair[1]) {

		chrptr_pair[1]++;

	}

	if (**chrptr_pair || *chrptr_pair[1]) {

		*abcd_pair &= 95;
		abcd_pair[1] &= 95;
		side = 1;
		goto ii_match;

	}

	return _LIBCONFINI_TRUE_;

}


												/**  @utility{ini_array_match}  **/
/**

	@brief			Compares two INI arrays and checks if they match
	@param			ini_string_a	The first INI array
	@param			ini_string_b	The second INI array
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			A boolean: `true` if the two arrays match, `false` otherwise

	INI strings are the strings typically dispatched by #load_ini_file() and
	#load_ini_path(), which may contain quotes and the three escape sequences `\\`,
	`\'` and `\"`.

	In order to be suitable for both names and values, **this function always
	considers sequences of one or more spaces out of quotes in both strings as
	collapsed**, even when `format.do_not_collapse_values` is set to `true`.

	This function can be used, with `'.'` as delimiter, to compare section paths.

	This function grants that the result of the comparison between two INI arrays
	will always match the the _literal_ comparison between the individual members
	of both arrays after these have been parsed, one by one, by #ini_string_parse()
	(with `format.do_not_collapse_values` set to `false`).

	The @p format argument is used for the following fields:

	- `format.case_sensitive`
	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes`

**/
_Bool ini_array_match (
	const char * const ini_string_a,
	const char * const ini_string_b,
	const char delimiter,
	const IniFormat format
) {

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*

			We have no delimiters (array has only one member)

		*/

		return ini_string_match_ii(ini_string_a, ini_string_b, format);

	}

	const _LIBCONFINI_BOOL_ has_escape = !INIFORMAT_HAS_NO_ESC(format);
	register uint8_t side = 1;
	register _LIBCONFINI_BOOL_ turn_allowed = _LIBCONFINI_TRUE_;
	uint8_t abcd_pair[2];
	size_t nbacksl_pair[2];
	const char * chrptr_pair[2] = {
		ini_string_a + ltrim_s(ini_string_a, 0, _LIBCONFINI_WITH_EOL_),
		ini_string_b + ltrim_s(ini_string_b, 0, _LIBCONFINI_WITH_EOL_)
	};

	/*

	Masks `abcd_pair[0]` and  `abcd_pair[1]` (8 bits used):

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

	abcd_pair[1] = *abcd_pair = 32 | (format.no_double_quotes << 1) | format.no_single_quotes;


    /* \                                /\
    \ */     delimited_match:          /* \
     \/     ______________________     \ */


	nbacksl_pair[side] = 0;

	if (has_escape && *chrptr_pair[side] == _LIBCONFINI_BACKSLASH_) {

		for (nbacksl_pair[side]++; *(++chrptr_pair[side]) == _LIBCONFINI_BACKSLASH_; nbacksl_pair[side]++);

		abcd_pair[side] = nbacksl_pair[side] & 1 ? (abcd_pair[side] & 31) | 16 : abcd_pair[side] & 15;

		if (
			(
				(abcd_pair[side] & 9) || *chrptr_pair[side] != _LIBCONFINI_SINGLE_QUOTES_
			) && (
				(abcd_pair[side] & 6) || *chrptr_pair[side] != _LIBCONFINI_DOUBLE_QUOTES_
			)
		) {

			nbacksl_pair[side]++;

		}

	} else {

		abcd_pair[side]	=	!(abcd_pair[side] & 12) && is_some_space(*chrptr_pair[side], _LIBCONFINI_WITH_EOL_) ?
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

	if (*nbacksl_pair || nbacksl_pair[1]) {

		if (*nbacksl_pair >> 1 != nbacksl_pair[1] >> 1) {

			return _LIBCONFINI_FALSE_;

		}

		side = 1;
		goto delimited_match;

	}

	if ((*abcd_pair ^ abcd_pair[1]) & 64) {

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

	if (!(*abcd_pair & 64)) {

		if (
			format.case_sensitive ?
				**chrptr_pair != *chrptr_pair[1]
			:
				_LIBCONFINI_CHR_CASEFOLD_(**chrptr_pair) != _LIBCONFINI_CHR_CASEFOLD_(*chrptr_pair[1])
		) {

			return _LIBCONFINI_FALSE_;

		}

		*abcd_pair &= 223;
		abcd_pair[1] &= 223;

	}

	if (**chrptr_pair) {

		(*chrptr_pair)++;

	}

	if (*chrptr_pair[1]) {

		chrptr_pair[1]++;

	}

	if (**chrptr_pair || *chrptr_pair[1]) {

		side = 1;
		goto delimited_match;

	}

	return _LIBCONFINI_TRUE_;

}


													/**  @utility{ini_unquote}  **/
/**

	@brief			Unescapes `\'`, `\"`, and `\\` and removes all unescaped quotes
					(if single/double quotes are considered metacharacters in
					respect to the format given)
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

	This function is very similar to #ini_string_parse(), except that does not
	bother collapsing the sequences of more than one space that might result from
	removing empty quotes. Its purpose is to be used to parse key and section names,
	since these are always dispatched as already collapsed. In order to parse
	values, or array parts listed in values, please use #ini_string_parse() instead.

	If you only need to compare @p ini_string with another string, consider to use
	#ini_string_match_si() and #ini_string_match_ii() instead of parsing the former
	and perform a simple comparison afterwards. These two functions are in fact able
	to check directly for equality between unparsed INI strings without actually
	modifiyng them.

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well). If the string does not contain quotes, or if quotes are
	considered to be normal characters, no changes will be made.

	The @p format argument is used for the following fields:

	- `format.no_single_quotes`
	- `format.no_double_quotes`
	- `format.multiline_nodes`

	@include topics/ini_string_parse.c

**/
size_t ini_unquote (char * const ini_string, const IniFormat format) {

	register size_t idx = 0;

	if (INIFORMAT_HAS_NO_ESC(format)) {

		/*

			There are no escape sequences... I will just return the length of the
			string.

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

	for (register uint8_t abcd = (format.no_double_quotes << 1) | format.no_single_quotes; ini_string[idx]; idx++) {

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


												/**  @utility{ini_string_parse}  **/
/**

	@brief			Unescapes `\'`, `\"`, and `\\` and removes all unescaped quotes
					(if single/double quotes are considered metacharacters in
					respect to the format given); if the format allows it, sequences
					of one or more spaces out of quotes will be collapsed
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

	This function is meant to be used to parse values. In order to parse key and
	section names please use #ini_unquote() instead.

	If you only need to compare @p ini_string with another string, consider to use
	#ini_string_match_si() and #ini_string_match_ii() instead of parsing the former
	and perform a simple comparison afterwards. These two functions are in fact able
	to check directly for equality between unparsed INI strings without actually
	modifying them.

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well). If `format.do_not_collapse_values` is set to non-zero, spaces
	surrounding empty quotes will be collapsed together with the latter.

	The @p format argument is used for the following fields:

	- `format.no_single_quotes`
	- `format.no_double_quotes`
	- `format.multiline_nodes`
	- `format.do_not_collapse_values`

	@note	`format.multiline_nodes` is used only to figure out whether there are
			escape sequences or not. For all other purposes new line characters will
			be considered to be equal to any other space character, even if the
			format is not multi-line -- in fact new line characters should never
			appear in non-multi-line formats .

	@include topics/ini_string_parse.c

**/
size_t ini_string_parse (char * const ini_string, const IniFormat format) {

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

	register uint8_t abcd	=	(format.do_not_collapse_values ? 68 : 64) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;

	size_t idx, lshift;

	if (format.multiline_nodes == INI_NO_MULTILINE) {

		switch (abcd) {

			case 67:

				/*

					There are no escape sequences but spaces might still need to
					be collapsed.

				*/

				for (idx = 0, lshift = 0; ini_string[idx]; idx++) {

					abcd = !is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_) ? 3 : abcd & 64 ? 195 : 67;

					if (abcd & 128) {

						lshift++;

					} else {

						ini_string[idx - lshift] = abcd & 64 ? _LIBCONFINI_COLLAPSED_ : ini_string[idx];

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

			case 71:

				/*

					There are no escape sequences and spaces don't need to be
					collapsed, but left and right trim might still be necessary.

				*/

				return rtrim_h(ini_string, ltrim_hh(ini_string, 0, _LIBCONFINI_WITH_EOL_), _LIBCONFINI_WITH_EOL_);

		}

	}

	/*

		There might be escape sequences

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

		ini_string[idx - lshift] = abcd & 64 ? _LIBCONFINI_COLLAPSED_ : ini_string[idx];

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


											/**  @utility{ini_array_get_length}  **/
/**

	@brief			Gets the length of a stringified INI array in number of members
	@param			ini_string		The stringified array (it can be `NULL`)
	@param			delimiter		The delimiter between the array members -- if
									zero (see #INI_ANY_SPACE), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The length of the INI array

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

**/
size_t ini_array_get_length (const char * const ini_string, const char delimiter, const IniFormat format) {

	if (!ini_string) {

		return 0;

	}

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*

			We have no delimiters (array has only one member)

		*/

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

	register uint8_t abcd	=	(delimiter ? 64 : 68) |
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


											/**  @utility{ini_array_foreach}  **/
/**

	@brief			Calls a custom function for each member of a stringified INI
					array, without modifying the content of the buffer -- useful for
					read-only (`const`) stringified arrays
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

	The function @p f_foreach (see #IniSubstrHandler data type) will be invoked with
	six arguments: `ini_string`, `memb_offset` (the offset of the member in bytes),
	`memb_length` (the length of the member in bytes), `memb_num` (the offset of the
	member in number of members), `format` (the format of the INI file), `user_data`
	(the custom argument @p user_data previously passed). If @p f_foreach returns a
	non-zero value the function #ini_array_foreach() will be interrupted.

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_foreach.c.

**/
int ini_array_foreach (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format,
	const IniSubstrHandler f_foreach,
	void * const user_data
) {

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

		/*

			We have no delimiters (array has only one member)

		*/

		idx = 0;

		while (ini_string[idx++]);

		return	f_foreach(ini_string, offs, rtrim_s(ini_string + offs, idx - offs - 1, _LIBCONFINI_WITH_EOL_), 0, format, user_data) ?
					CONFINI_FEINTR
				:
					CONFINI_SUCCESS;

	}

	register uint8_t abcd	=	(delimiter ? 4 : 0) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;

	size_t memb_num = 0;

	idx = offs;

	do {

		abcd	=	(delimiter ? ini_string[idx] == delimiter : is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_)) ?
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

			offs = abcd & 128 ? idx + 1 : ltrim_s(ini_string, idx + 1, _LIBCONFINI_WITH_EOL_);

		}

		idx = abcd & 216 ? idx + 1 : offs;

	} while (!(abcd & 128) && ((abcd & 92) || ini_string[idx]));

	return CONFINI_SUCCESS;

}


												/**  @utility{ini_array_shift}  **/
/**

	@brief			Shifts the location pointed by @p ini_strptr to the next member
					of the INI array (without modifying the content of the buffer),
					or to `NULL` if the INI array has no more members  -- useful for
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
size_t ini_array_shift (const char ** const ini_strptr, const char delimiter, const IniFormat format) {

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


											/**  @utility{ini_array_collapse}  **/
/**

	@brief			Compresses the distribution of the data of a stringified INI
					array by removing all the white spaces that surround its
					delimiters, empty quotes, collapsable spaces, etc
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

	1. Using the comma as delimiter:
	   - Before: `&nbsp;first&nbsp;&nbsp; ,&nbsp;&nbsp;&nbsp; second&nbsp;&nbsp;
	     ,&nbsp;&nbsp; third&nbsp;&nbsp; ,&nbsp; etc.&nbsp;&nbsp;`
	   - After: `first,second,third,etc.`
	2. Using `INI_ANY_SPACE` as delimiter:
	   - Before: `&nbsp;&nbsp;first&nbsp;&nbsp;&nbsp; second&nbsp;&nbsp;&nbsp;
	     third&nbsp;&nbsp;&nbsp;&nbsp; etc.&nbsp;&nbsp;&nbsp;`
	   - After: `first second third etc.`

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_collapse.c

	@note	The actual space occupied by the array might get reduced further after
			each member is parsed by #ini_string_parse().

**/
size_t ini_array_collapse (char * const ini_string, const char delimiter, const IniFormat format) {

	if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

		/*

			We have no delimiters (array has only one member)

		*/

		switch ((format.preserve_empty_quotes << 1) | format.do_not_collapse_values) {

			case 0: return collapse_everything(ini_string, format);
			case 1: return collapse_empty_quotes(ini_string, format);
			case 2: return collapse_spaces(ini_string, format);
			case 3: return rtrim_h(ini_string, ltrim_hh(ini_string, 0, _LIBCONFINI_WITH_EOL_), _LIBCONFINI_WITH_EOL_);

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

	register uint16_t abcd		=	(delimiter ? 0 : 16) |
									(format.preserve_empty_quotes << 3) |
									(format.do_not_collapse_values << 2) |
									(format.no_double_quotes << 1) |
									format.no_single_quotes;


	for (; ini_string[idx_s]; idx_s++) {

		/*  Revision #1  */

		abcd	=	!(abcd & 112) && ini_string[idx_s] == delimiter ?
						(
							(abcd & 536) && ((abcd & 1560) ^ 8) && ((abcd & 1560) ^ 1544) && ((abcd & 1304) ^ 1032) ?
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
							: !((abcd & 536) ^ 528) || !((abcd & 1560) ^ 536) || !((abcd & 1560) ^ 1048) ?
								(abcd & 32895) | 32768
							: !(abcd & 540) || !((abcd & 1564) ^ 8) || !((abcd & 536) ^ 16) || !((abcd & 1560) ^ 24) ?
								abcd & 2431
							: ((abcd & 540) ^ 4) && ((abcd & 796) ^ 12) && ((abcd & 1564) ^ 12) && ((abcd & 1308) ^ 1032) ?
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
							(abcd & 888) && ((abcd & 1144) ^ 1032) && ((abcd & 1144) ^ 1048) && ((abcd & 2936) ^ 8) ?
								((abcd & 33791) | 33536) ^ 128
							:
								((abcd & 41983) | 41728) ^ 128
						)
					: (abcd & 888) && ((abcd & 1144) ^ 1032) && ((abcd & 1144) ^ 1048) && ((abcd & 2936) ^ 8) ?
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
					: !(abcd & 536) || !(~abcd & 1544) || !((abcd & 1560) ^ 8) || !((abcd & 1304) ^ 1032) ?
						(idx_d = fallback)
					: !((abcd & 1624) ^ 1104) || !((abcd & 1592) ^ 1072) ?
						(idx_d -= 2)
					: ((abcd & 1552) ^ 16) && ((abcd & 632) ^ 16) && ((abcd & 1624) ^ 1616) && ((abcd & 1592) ^ 1584) ?
						idx_d
					:
						--idx_d;

			ini_string[idx_s];

		ini_string[idx_s++] = '\0'

	);

	return idx_d;

}


												/**  @utility{ini_array_break}  **/
/**

	@brief			Replaces the first delimiter found (together with the spaces
					that surround it) with `\0`
	@param			ini_string		The stringified array -- it can be `NULL`
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

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_break.c

**/
char * ini_array_break (char * const ini_string, const char delimiter, const IniFormat format) {

	if (ini_string) {

		char * remnant;

		if (_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

			/*

				We have no delimiters (array has only one member)

			*/

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


											/**  @utility{ini_array_release}  **/
/**

	@brief			Replaces the first delimiter found (together with the spaces
					that surround it) with `\0`, then shifts the location pointed by
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

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_release.c

**/
char * ini_array_release (char ** const ini_strptr, const char delimiter, const IniFormat format) {

	char * const token = *ini_strptr;

	if (token && !_LIBCONFINI_IS_ESC_CHAR_(delimiter, format)) {

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


												/**  @utility{ini_array_split}  **/
/**

	@brief			Splits a stringified INI array into NUL-separated members and
					calls a custom function for each member
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

	The function @p f_foreach (see #IniStrHandler data type) will be invoked with
	five arguments: `member` (the member of the array), `memb_length` (the length of
	the member in bytes), `memb_num` (the offset of the member in number of
	members), `format` (the format of the INI file), `user_data` (the custom
	argument @p user_data previously passed). If @p f_foreach returns a non-zero
	value the function #ini_array_split() will be interrupted.

	Similarly to `strtok_r()` this function can be used only once for a given
	string.

	@note	If @p delimiter matches a metacharacter within the format given (`'\\'`,
			`'\''` or `'\"'`), its role as metacharacter will have higher priority
			than its role as delimiter (i.e., the array will have no delimiters and
			will contain only one member).

	@include topics/ini_array_split.c.

**/
int ini_array_split (
	char * const ini_string,
	const char delimiter,
	const IniFormat format,
	const IniStrHandler f_foreach,
	void * const user_data
) {

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

		/*

			We have no delimiters (array has only one member)

		*/

		idx = 0;

		while (ini_string[idx++]);

		return	f_foreach(ini_string + offs, rtrim_h(ini_string + offs, idx - offs - 1, _LIBCONFINI_WITH_EOL_), 0, format, user_data) ?
					CONFINI_FEINTR
				:
					CONFINI_SUCCESS;

	}

	register uint8_t abcd	=	(delimiter ? 4 : 0) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;

	size_t memb_num = 0;

	idx = offs;


	do {

		abcd	=	(delimiter ? ini_string[idx] == delimiter : is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_)) ?
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

			offs = abcd & 128 ? idx + 1 : ltrim_h(ini_string, idx + 1, _LIBCONFINI_WITH_EOL_);

		}

		idx = abcd & 216 ? idx + 1 : offs;

	} while (!(abcd & 128) && ((abcd & 92) || ini_string[idx]));

	return CONFINI_SUCCESS;

}


								/**  @utility{ini_global_set_lowercase_mode}  **/
/**

	@brief			Sets the value of the global variable
					#INI_GLOBAL_LOWERCASE_MODE
	@param			lowercase		The new value
	@return			Nothing

	If @p lowercase is `true`, key and section names in case-insensitive INI formats
	will be dispatched lowercase, verbatim otherwise (default value: `true`).

	@warning	This function changes the value of one or more global variables. In
				order to be thread-safe this function should be used only once at
				beginning of execution, or otherwise a mutex logic must be
				introduced.

**/
void ini_global_set_lowercase_mode (_Bool lowercase) {

	INI_GLOBAL_LOWERCASE_MODE = lowercase;

}


								/**  @utility{ini_global_set_implicit_value}  **/
/**

	@brief			Sets the value to be to be assigned to implicit keys
	@param			implicit_value		The string to be used as implicit value
										(usually `"YES"`, or `"TRUE"`)
	@param			implicit_v_len		The length of @p implicit_value (usually
										`0`, independently of its real length)
	@return			Nothing

	@warning	This function changes the value of one or more global variables. In
				order to be thread-safe this function should be used only once at
				beginning of execution, or otherwise a mutex logic must be
				introduced.

	@include topics/ini_global_set_implicit_value.c

**/
void ini_global_set_implicit_value (char * const implicit_value, const size_t implicit_v_len) {

	INI_GLOBAL_IMPLICIT_VALUE = implicit_value;
	INI_GLOBAL_IMPLICIT_V_LEN = implicit_v_len;

}


														/**  @utility{ini_fton}  **/
/**

	@brief			Calculates the #IniFormatNum of an #IniFormat
	@param			source			The #IniFormat to compute
	@return			The unique unsigned integer that identifies the format given

**/
IniFormatNum ini_fton (const IniFormat source) {

	#define __INIFORMAT_PROPERTIES__(NAME, OFFSET, SIZE, DEFVAL) (source.NAME << OFFSET) |

	return INIFORMAT_TABLE_AS(__INIFORMAT_PROPERTIES__) 0;

	#undef __INIFORMAT_PROPERTIES__

}


														/**  @utility{ini_ntof}  **/
/**

	@brief			Constructs a new #IniFormat according to an #IniFormatNum
	@param			format_num		The #IniFormatNum to parse
	@return			The new #IniFormat constructed

	@note	If @p format_num `>` `16777215` it will be truncated to 24 bits.

**/
IniFormat ini_ntof (IniFormatNum format_num) {

	#define __MAX_1_BITS__ 1
	#define __MAX_2_BITS__ 3
	#define __MAX_3_BITS__ 7
	#define __MAX_4_BITS__ 15
	#define __MAX_5_BITS__ 31
	#define __MAX_6_BITS__ 63
	#define __MAX_7_BITS__ 127
	#define __MAX_8_BITS__ 255
	#define __INIFORMATNUM_FLAGS__(NAME, OFFSET, SIZE, DEFVAL) \
		(format_num >> OFFSET) & __MAX_##SIZE##_BITS__,

	return (IniFormat) { INIFORMAT_TABLE_AS(__INIFORMATNUM_FLAGS__) };

	#undef __INIFORMATNUM_FLAGS__
	#undef __MAX_8_BITS__
	#undef __MAX_7_BITS__
	#undef __MAX_6_BITS__
	#undef __MAX_5_BITS__
	#undef __MAX_4_BITS__
	#undef __MAX_3_BITS__
	#undef __MAX_2_BITS__
	#undef __MAX_1_BITS__

}


													/**  @utility{ini_get_bool}  **/
/**

	@brief			Checks whether a string matches one of the booleans listed in
					the private constant #INI_BOOLEANS (case-insensitive)
	@param			ini_string			A string to be checked
	@param			return_value		A value that is returned if no matching
										boolean has been found
	@return			The matching boolean value (0 or 1) or @p return_value if no
					boolean has been found

	Usually @p ini_string comes from an #IniDispatch (but any other string may be
	used as well).

	@include miscellanea/typed_ini.c

**/
int ini_get_bool (const char * const ini_string, const int return_value) {

	uint8_t bool_idx;
	size_t pair_idx, chr_idx;

	for (pair_idx = 0; pair_idx < sizeof(INI_BOOLEANS) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			chr_idx = 0;

			while (_LIBCONFINI_CHR_CASEFOLD_(ini_string[chr_idx]) == INI_BOOLEANS[pair_idx][bool_idx][chr_idx]) {

				if (!ini_string[chr_idx++]) {

					return (int) bool_idx;

				}

			}

		}

	}

	return return_value;

}



		/*  LINKS -- In case you don't have `#include <stdlib.h>` in your source  */


/**

	@alias{ini_get_int}
		Link to [`atoi()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi)
	@alias{ini_get_lint}
		Link to [`atol()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol)
	@alias{ini_get_llint}
		Link to [`atoll()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll)
	@alias{ini_get_float}
		Link to [`atof()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof)

**/

int (* const ini_get_int) (const char * ini_string) = &atoi;

long int (* const ini_get_lint) (const char * ini_string) = &atol;

long long int (* const ini_get_llint) (const char * ini_string) = &atoll;

double (* const ini_get_float) (const char * ini_string) = &atof;



		/*  GLOBAL VARIABLES  */


_Bool INI_GLOBAL_LOWERCASE_MODE = _LIBCONFINI_FALSE_;

char * INI_GLOBAL_IMPLICIT_VALUE = (char *) 0;

size_t INI_GLOBAL_IMPLICIT_V_LEN = 0;



/**  @endfnlist  **/

/*  EOF  */

