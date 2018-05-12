/*	-*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-	*/

/**

	@file		confini.c
	@brief		libconfini functions
	@author		Stefano Gioffr&eacute;
	@copyright	GNU Public License v3
	@date		2016-2018
	@see		Source code at https://github.com/madmurphy/libconfini/blob/master/src/confini.c?ts=4

**/

#include <stdio.h>
#include <stdlib.h>
#include "confini.h"


/**


	@struct		IniFormat

	Structure of the bitfield:

	- Bits 1-19: INI syntax
	- Bits 20-22: INI semantics
	- Bits 23-24: Human syntax (disabled entries)

	@property	IniFormat::delimiter_symbol
					The symbol to be used as delimiter (only ASCII allowed); if set
					to `0`, any space is delimiter
					(`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@property	IniFormat::case_sensitive
					If set to `1`, string comparisons will always be performed
					case-sensitive
	@property	IniFormat::semicolon_marker
					The rule of the semicolon character (use enum
					`#IniCommentMarker` for this)
	@property	IniFormat::hash_marker
					The rule of the hash character (use enum `#IniCommentMarker` for
					this)
	@property	IniFormat::section_paths
					Defines whether and how the format supports sections (use enum
					`#IniSectionPaths` for this)
	@property	IniFormat::multiline_nodes
					Defines which class of entries are allowed to be multi-line (use
					enum `#IniMultiline` for this)
	@property	IniFormat::no_spaces_in_names
					If set to `1`, key and section names containing spaces (even
					within quotes) will be rendered as `#INI_UNKNOWN`. Note that
					setting `IniFormat::delimiter_symbol` to `INI_ANY_SPACE` will
					not automatically set this option to `1` (spaces will still be
					allowed in section names).
	@property	IniFormat::no_single_quotes
					If set to `1`, the single-quote character (`'`) will be
					considered as a normal character
	@property	IniFormat::no_double_quotes
					If set to `1`, the double-quote character (`"`) will be
					considered as a normal character
	@property	IniFormat::implicit_is_not_empty
					If set to `1`, implicit keys (see @ref libconfini) will always
					be dispatched using the values of the global variables
					`#INI_GLOBAL_IMPLICIT_VALUE` and `#INI_GLOBAL_IMPLICIT_V_LEN`
					for the fields `IniDispatch::value` and to `IniDispatch::v_len`
					respectively; if set to `0`, implicit keys will be considered
					to be empty keys
	@property	IniFormat::do_not_collapse_values
					If set to `1`, sequences of more than one space in values
					(`/\s{2,}/`) will not be collapsed
	@property	IniFormat::preserve_empty_quotes
					If set to `1` and if single/double quotes are metacharacters
					ensures that, within values, empty strings enclosed between
					quotes (`""` or `''`) will not be collapsed together with the
					spaces that surround them. This option is useful for values
					containing space-delimited arrays in order to preserve their
					empty members -- as in, for instance: `coordinates = "" ""`.
					Note that, in section and key names, empty strings enclosed
					between quotes are _always_ collapsed together with their
					surrounding spaces.
	@property	IniFormat::no_disabled_after_space
					If set to `1`, prevents that what follows `/(?:^|\s+)[#;]\s/` be
					parsed as a disabled entry
	@property	IniFormat::disabled_can_be_implicit
					If set to `1`, comments non containing a delimiter symbol will
					not be parsed as disabled implicit keys, but as simple comments


	@struct		IniStatistics

	@property	IniStatistics::format
					The format of the INI file (see struct `IniFormat`)
	@property	IniStatistics::bytes
					The size of the parsed file in bytes
	@property	IniStatistics::members
					The size of the parsed file in members (nodes) -- this number
					equals the number of dispatches


	@struct		IniDispatch

	@property	IniDispatch::format
					The format of the INI file (see struct `IniFormat`)
	@property	IniDispatch::type
					The dispatch type (see enum `#IniNodeType`)
	@property	IniDispatch::data
					It can be the content of a comment, a section path or a key name
	@property	IniDispatch::value
					It can be the value of a key element or an empty string
	@property	IniDispatch::append_to
					The current section path
	@property	IniDispatch::d_len
					The length of the string `IniDispatch::data`
	@property	IniDispatch::v_len
					The length of the string `IniDispatch::value`
	@property	IniDispatch::at_len
					The length of the string `IniDispatch::append_to`
	@property	IniDispatch::dispatch_id
					The dispatch id


**/

/** @startfnlist **/



		/*\
		|*|
		|*|   PRIVATE ENVIRONMENT
		|*|
		\*/



		/* ALIASES */


#define _LIBCONFINI_FALSE_ 0
#define _LIBCONFINI_TRUE_ 1
#define _LIBCONFINI_SUCCESS_ 0
#define _LIBCONFINI_BACKSLASH_ '\\'
#define _LIBCONFINI_OPEN_SECTION_ '['
#define _LIBCONFINI_CLOSE_SECTION_ ']'
#define _LIBCONFINI_SUBSECTION_ '.'
#define _LIBCONFINI_SEMICOLON_ ';'
#define _LIBCONFINI_HASH_ '#'
#define _LIBCONFINI_DOUBLE_QUOTES_ '"'
#define _LIBCONFINI_SINGLE_QUOTES_ '\''
#define _LIBCONFINI_SIMPLE_SPACE_ 32
#define _LIBCONFINI_HT_ '\t'
#define _LIBCONFINI_FF_ '\f'
#define _LIBCONFINI_VT_ '\v'
#define _LIBCONFINI_CR_ '\r'
#define _LIBCONFINI_LF_ '\n'

/*
	This may be any character, in theory... But after the left-trim of each line a
	leading space works pretty well as metacharacter...
*/
#define _LIBCONFINI_INLINE_MARKER_ _LIBCONFINI_SIMPLE_SPACE_

/* `_LIBCONFINI_BOOL_` is for internal usage only... */
#define _LIBCONFINI_BOOL_ unsigned char



		/* FUNCTIONAL MACROS AND CONSTANTS */


/*
	Maybe in the future there will be support for UTF-8 casefold, but for now only
	ASCII...
*/
#define _LIBCONFINI_CHR_CASEFOLD_(CHR) (CHR > 0x40 && CHR < 0x5b ? CHR | 0x60 : CHR)

/*
	Possible depths of `_LIBCONFINI_SPACES_` (see function `is_some_space()`).
	Please, consider the three following constants as belonging together to a
	virtual opaque `enum`.
*/
#define _LIBCONFINI_WITH_EOL_ -1
#define _LIBCONFINI_NO_EOL_ 1
#define _LIBCONFINI_JUST_S_T_ 3

/* Other constants related to `_LIBCONFINI_SPACES_` */
#define _LIBCONFINI_EOL_IDX_ 0
#define _LIBCONFINI_SPALEN_ 6

/* The list of space characters -- do not change its order! */
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

	Each pair must be presented in this order:

	1. Signifier of `FALSE`
	2. Signifier of `TRUE`.

	There may be infinite pairs here, but be aware of the lazy behavior	of
	`ini_get_lazy_bool()`. 

	Everything lowercase in this list!

**/
static const char * const INI_BOOLEANS[][2] = {
	{ "no", "yes" },
	{ "false", "true" },
	{ "0", "1" }
};



		/* ABSTRACT UTILITIES */


/**

	@brief			Checks whether a character is a space
	@param			chr				The target character
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			A boolean: `TRUE` if the character matches, `FALSE` otherwise

**/
static inline _LIBCONFINI_BOOL_ is_some_space (const char chr, const uint8_t depth) {
	int8_t idx = depth;
	while (++idx < _LIBCONFINI_SPALEN_ && chr != _LIBCONFINI_SPACES_[idx]);
	return idx < _LIBCONFINI_SPALEN_;
}


/**

	@brief			Soft left trim -- does **not** change the buffer
	@param			ltstr			The target string
	@param			start_from		The offset where to start the left trim
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The offset of the first non-space character

**/
static inline size_t ltrim_s (const char * const ltstr, const size_t start_from, const uint8_t depth) {
	size_t idx = start_from;
	while (ltstr[idx] && is_some_space(ltstr[idx], depth)) { idx++; }
	return idx;
}


/**

	@brief			Hard left trim -- **does** change the buffer
	@param			ltstr			The target string
	@param			start_from		The offset where to start the left trim
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The offset of the first non-space character

**/
static inline size_t ltrim_h (char * const ltstr, const size_t start_from, const uint8_t depth) {
	size_t idx = start_from;
	while (ltstr[idx] && is_some_space(ltstr[idx], depth)) { ltstr[idx++] = '\0'; }
	return idx;
}


/**

	@brief			Soft right trim -- does **not** change the buffer
	@param			rtstr			The target string
	@param			length			The length of the string
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The length of the string until the last non-space character

**/
static inline size_t rtrim_s (const char * const rtstr, const size_t length, const uint8_t depth) {
	size_t newlen = length;
	while (newlen > 0 && is_some_space(rtstr[newlen - 1], depth)) { newlen--; }
	return newlen;
}


/**

	@brief			Hard right trim -- **does** change the buffer
	@param			rtstr			The target string
	@param			length			The length of the string
	@param			depth			What is actually considered a space (possible
									values: `_LIBCONFINI_WITH_EOL_`,
									`_LIBCONFINI_NO_EOL_`, `_LIBCONFINI_JUST_S_T_`)
	@return			The new length of the string

**/
static inline size_t rtrim_h (char * const rtstr, const size_t length, const uint8_t depth) {
	size_t newlen = length;
	while (newlen > 0 && is_some_space(rtstr[newlen - 1], depth)) { rtstr[--newlen] = '\0'; }
	return newlen;
}


/**

	@brief			Unescaped soft right trim (right trim of `/(\s+|\\[\n\r])+$/`)
					-- does **not** change the buffer
	@param			urtstr			The target string
	@param			length			The length of the string
	@return			The length of the string until the last non-space character

**/
static inline size_t urtrim_s (const char * const urtstr, const size_t length) {

	register uint8_t abcd = 1;
	size_t idx = length;

	/* ====> */   continue_trim:   /* <==== */

	if (idx < 1) {

		return idx;

	}

	switch (urtstr[--idx]) {

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
	@param			string		The target string
	@return			Nothing

**/
static inline void string_tolower (char * const str) {
	for (char *ch_ptr = str; *ch_ptr; ch_ptr++) {
		*ch_ptr = _LIBCONFINI_CHR_CASEFOLD_(*ch_ptr);
	}
}



		/* CONCRETE UTILITIES */


/**

	@brief			Checks whether a character can represent the beginning of a
					disabled entry within a given format
	@param			chr			The character to check
	@param			format		The format of the INI file
	@return			1 if @p chr matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_parsable_char (const char chr, const IniFormat format) {

	return	(
				chr == _LIBCONFINI_SEMICOLON_ && format.semicolon_marker == INI_DISABLED_OR_COMMENT
			) || (
				chr == _LIBCONFINI_HASH_ && format.hash_marker == INI_DISABLED_OR_COMMENT
			);

}


/**

	@brief			Checks whether a character represents the opening of a comment
					within a given format
	@param			chr				The character to check
	@param			format			The format of the INI file
	@return			1 if @p chr matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_comment_char (const char chr, const IniFormat format) {

	return	(
				chr == _LIBCONFINI_SEMICOLON_ && format.semicolon_marker != INI_IS_NOT_A_MARKER
			) || (
				chr == _LIBCONFINI_HASH_ && format.hash_marker != INI_IS_NOT_A_MARKER
			);

}


/**

	@brief			Checks whether a character represents the beginning of a
					comment that must be ignored within a given format
	@param			chr			The character to check
	@param			format		The format of the INI file
	@return			1 if @p chr matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_forget_char (const char chr, const IniFormat format) {

	return	(
				format.semicolon_marker == INI_IGNORE && chr == _LIBCONFINI_SEMICOLON_
			) || (
				format.hash_marker == INI_IGNORE && chr == _LIBCONFINI_HASH_
			);

}


/**

	@brief			Unparsed hard left trim (left trim of
					`/^(\s+|\\[\n\r]|''|"")+/`) -- **does** change the buffer
	@param			qultstr			The target string
	@param			start_from		The offset where to start the left trim
	@return			The offset of the first non-space character

**/
static inline size_t qultrim_h (char * const qultstr, const size_t start_from, const IniFormat format) {

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
	size_t idx = start_from;

	for (; abcd & 128; idx++) {

		abcd	= 	!(abcd & 28) && is_some_space(qultstr[idx], _LIBCONFINI_NO_EOL_) ?
						(abcd & 207) | 64
					: !(abcd & 12) && (qultstr[idx] == _LIBCONFINI_LF_ || qultstr[idx] == _LIBCONFINI_CR_) ?
						(
							 abcd & 16 ?
								(abcd & 239) | 96
							:
								abcd | 64
						)
					: !(abcd & 25) && qultstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							abcd & 4 ?
								(abcd & 235) | 96
							:
								(abcd & 143) | 4
						)
					: !(abcd & 22) && qultstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							abcd & 8 ?
								(abcd & 231) | 96
							:
								(abcd & 159) | 8
						)
					: qultstr[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd & 159) ^ 16
					:
						abcd & 31;


		if (abcd & 32) {

			qultstr[idx - 1] = '\0';

		}

		if (abcd & 64) {

			qultstr[idx] = '\0';

		}

	}

	return abcd & 28 ? idx - 2 : idx - 1;

}


/**

	@brief			Gets the position of the first delimiter out of quotes
	@param			pairstr			The string to where to search for the delimiter
	@param			length			The length of the string
	@param			format			The format of the INI file
	@return			The offset of the delimiter or @p length if the delimiter has
					not been not found

**/
static inline size_t get_delimiter_pos (const char * const pairstr, const size_t len, const IniFormat format) {

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

			idx < len && (
				(abcd & 12) || !(
					format.delimiter_symbol ?
						pairstr[idx] == format.delimiter_symbol
						: is_some_space(pairstr[idx], _LIBCONFINI_WITH_EOL_)
				)
			);

		abcd	=	pairstr[idx] == _LIBCONFINI_BACKSLASH_ ? abcd ^ 16
					: !(abcd & 22) && pairstr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abcd ^ 8
					: !(abcd & 25) && pairstr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abcd ^ 4
					: abcd & 15,
		idx++

	);

	return idx;

}


/**

	@brief			Replaces `/\\(\n\r?|\r\n?)s*[#;]/` or `/\\(\n\r?|\r\n?)/` with
					`"$1"`.
	@param			str					Target string
	@param			len					Length of the string
	@param			is_disabled			The string represents a disabled entry
	@param			format				The format of the INI file
	@return			The new length of the string

**/
static size_t unescape_cr_lf (char * const str, const size_t len, const _LIBCONFINI_BOOL_ is_disabled, const IniFormat format) {

	size_t iter, idx = 0, lshift = 0;

	for (_LIBCONFINI_BOOL_ is_escaped = _LIBCONFINI_FALSE_, cr_or_lf = _LIBCONFINI_EOL_IDX_; idx < len; idx++) {

		if (is_escaped && (str[idx] == _LIBCONFINI_SPACES_[cr_or_lf] || str[idx] == _LIBCONFINI_SPACES_[cr_or_lf ^= 1])) {

			for (

				iter = idx,
				idx += str[idx + 1] == _LIBCONFINI_SPACES_[cr_or_lf ^ 1];

					iter < idx + 1;

				str[iter - lshift - 1] = str[iter],
				iter++

			);

			lshift++;

			if (is_disabled) {

				iter = ltrim_s(str, iter, _LIBCONFINI_NO_EOL_);

				if (is_parsable_char(str[iter], format)) {

					lshift += iter - idx;
					idx = iter;

				}

			}

			is_escaped = _LIBCONFINI_FALSE_;

		} else {

			is_escaped = str[idx] == _LIBCONFINI_BACKSLASH_ ? !is_escaped : _LIBCONFINI_FALSE_;

			if (lshift) {

				str[idx - lshift] = str[idx];

			}

		} 

	}

	for (idx = len - lshift; idx < len; str[idx++] = '\0');

	return len - lshift;

}


/**

	@brief			Sanitizes a section path
	@param			secpath		The section path
	@param			format		The format of the INI file
	@return			The new length of the string

	Out of quotes, similar to ECMAScript
	`secpath.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/\s+/g, " ")`

	A section path can start with a dot (append), but cannot end with a dot. Spaces
	surrounding dots will be removed. Fragments surrounded by single or double
	quotes (if these are enabled) prevent changes.

**/
static size_t sanitize_section_path (char * const secpath, const IniFormat format) {

	size_t lshift = 0, idx = 0;

	/*

	Mask `abcd` (12 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		The string begins with spaces
		FLAG_64		This is a space out of quotes
		FLAG_128	This is a dot out of quotes
		FLAG_256	This is anything *but* an opening single/double quote
		FLAG_512	The quoted fragment is empty: remove it
		FLAG_1024	Increase the shift
		FLAG_2048	Path does not contain meaningful parts

	*/

	register uint16_t abcd = (format.no_double_quotes ? 290 : 288) | format.no_single_quotes;

	for (; secpath[idx]; idx++) {

		/* Revision #2 */

		abcd	=	!(abcd & 12) && is_some_space(secpath[idx], _LIBCONFINI_WITH_EOL_) ?
						(
							abcd & 224 ?
								(abcd & 3567) | 1344
							:
								(abcd & 2543) | 320
						)
					: !(abcd & 12) && secpath[idx] == _LIBCONFINI_SUBSECTION_ ?
						(
							abcd & (abcd & 32 ? 128 : 192) ?
								(abcd & 3471) | 1408
							:
								(abcd & 2447) | 384
						)
					: !(abcd & 25) && secpath[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							!(abcd & 4) ?
								(abcd & 2303) | 4
							: abcd & 256 ?
								(abcd & 2331) | 2304
							:
								(abcd & 3067) | 768
						)
					: !(abcd & 22) && secpath[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							!(abcd & 8) ?
								(abcd & 2303) | 8
							: abcd & 256 ?
								(abcd & 2327) | 2304
							:
								(abcd & 3063) | 768
						)
					: secpath[idx] == _LIBCONFINI_BACKSLASH_ ?
						((abcd & 2335) | 2304) ^ 16
					:
						(abcd & 2319) | 2304;


		if (abcd & 1536) {

			lshift++;

		}

		secpath[
			abcd & 512 ?
				idx - lshift++
			:
				idx - lshift
		]						=	!(~abcd & 384) ?
										_LIBCONFINI_SUBSECTION_
									: !(~abcd & 320) ?
										_LIBCONFINI_SIMPLE_SPACE_
									:
										secpath[idx];

	}

	for (idx -= lshift < idx && (abcd & 2048) && (abcd & 192) ? ++lshift : lshift; secpath[idx]; secpath[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Out of quotes similar to ECMAScript
					`string.replace(/''|""/g, "").replace(/^[\n\r]\s*|(\s)+/g, " ")`
	@param			keystr			The string to collapse
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t sanitize_key_name (char * const keystr, const IniFormat format) {

	/*

	Mask `abcd` (9 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		More than one space is here
		FLAG_64		This is an opening single/double quote
		FLAG_128	Jump this character
		FLAG_256	Jump this character and the one before this as well

	*/

	register uint16_t abcd	=	(is_some_space(*keystr, _LIBCONFINI_WITH_EOL_) ? 0 : 32) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;

	size_t idx = 0, lshift = 0;

	for (; keystr[idx]; idx++) {

		/* Revision #1 */

		abcd	=	!(abcd & 12) && is_some_space(keystr[idx], _LIBCONFINI_WITH_EOL_) ?
						(
							abcd & 32 ?
								abcd & 15
							:
								(abcd & 175) | 128
						)
					: !(abcd & 25) && keystr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(
							!(abcd & 4) ?
								(abcd & 111) | 68
							: abcd & 64 ?
								(abcd & 427) | 384
							:
								(abcd & 43) | 32
						)
					: !(abcd & 22) && keystr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(
							!(abcd & 8) ?
								(abcd & 111) | 72
							: abcd & 64 ?
								(abcd & 423) | 384
							:
								(abcd & 39) | 32
						)
					: keystr[idx] == _LIBCONFINI_BACKSLASH_ ?
						((abcd & 63) | 32) ^ 16
					:
						(abcd & 47) | 32;


		if (abcd & 384) {

				lshift += abcd & 256 ? 2 : 1;

		} else {

				keystr[idx - lshift] = abcd & 44 ? keystr[idx] : _LIBCONFINI_SIMPLE_SPACE_;

		}

	}

	for (idx -= abcd & 32 ? lshift : ++lshift; keystr[idx]; keystr[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Out of quotes similar to ECMAScript
					`string.replace(/^[\n\r]\s*|(\s)+/g, " ")`
	@param			str			The string to collapse
	@param			format		The format of the INI file
	@return			The new length of the string

**/
static size_t collapse_spaces (char * const str, const IniFormat format) {

	/*

	Mask `abcd` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		More than one space is here
		FLAG_64		Jump this character

	*/

	register uint8_t abcd = (format.no_double_quotes ? 34 : 32) | format.no_single_quotes;
	size_t idx = 0, lshift = 0;

	for (; str[idx]; idx++) {

		/* Revision #1 */

		abcd	=	!(abcd & 12) && is_some_space(str[idx], _LIBCONFINI_WITH_EOL_) ?
						(abcd & 32 ? (abcd & 111) | 64 : (abcd & 47) | 32)
					: !(abcd & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(abcd & 15) ^ 4
					: !(abcd & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(abcd & 15) ^ 8
					: str[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd & 31) ^ 16
					:
						abcd & 15;


		if (abcd & 64) {

				lshift++;

		} else {

				str[idx - lshift] = abcd & 32 ? _LIBCONFINI_SIMPLE_SPACE_ : str[idx];

		}

	}

	for (idx -= abcd & 32 ? ++lshift : lshift; str[idx]; str[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Removes all comment initializers (`#` or `;`) from the beginning
					of each line of a comment
	@param			commstr			The comment to parse
	@param			len				The length of commstr
	@param			format			The format of the INI file
	@return			The new length of the string

	- In multi-line comments: `commstr.replace(/(^|\n\r?|\r\n?)[\t \v\f]*[#;]+/g, "$1")`
	- In single-line comments: `commstr.replace(/^[\t \v\f]*[#;]+/, "")`

**/
static size_t uncomment (char * const commstr, size_t len, const IniFormat format) {

	size_t lshift, idx;

	if (format.multiline_nodes == INI_MULTILINE_EVERYWHERE) {

		/* The comment can be multi-line */

		register uint8_t abcd;

		/*

		Mask `abcd` (6 bits used):

			FLAG_1		Don't erase any character
			FLAG_2		We are in an odd sequence of backslashes
			FLAG_4		This character is a backslash
			FLAG_8		This character is a comment character and follows `/(\n\s*|\r\s*)/`
			FLAG_16		This character is a part of a group of spaces following
						a new line (`/(\n|\r)[\t \v\f]+/`)
			FLAG_32		This character is not a new line character (`/[^\r\n]/`)

		*/

		for (abcd = 11, lshift = 0, idx = 0; idx < len; idx++) {

			abcd	=	commstr[idx] == _LIBCONFINI_BACKSLASH_ ?
							((abcd & 35) | 32) ^ 2
						: commstr[idx] == _LIBCONFINI_LF_ || commstr[idx] == _LIBCONFINI_CR_ ?
							(abcd & 16) | ((abcd << 1) & 4)
						: !(abcd & 32) && is_comment_char(commstr[idx], format) ?
							(abcd & 40) | 8
						: !(abcd & 40) && is_some_space(commstr[idx], _LIBCONFINI_NO_EOL_) ?
							(abcd & 57) | 16
						:
							(abcd & 33) | 32;


			if (abcd & 28) {

				lshift++;

			}

			if (!(abcd & 25)) {

				commstr[idx - lshift] = commstr[idx];

			}

		}

	} else {

		/* The comment cannot be multi-line */

		for (lshift = 0; lshift < len && is_comment_char(commstr[lshift], format); lshift++);

		if (!lshift) {

			return len;

		}

		for (idx = lshift; idx < len; commstr[idx - lshift] = commstr[idx], idx++);

	}

	for (idx -= lshift; idx < len; commstr[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Tries to determine the type of a member "as if it was active"
	@param			nodestr				String containing an individual node
	@param			len					Length of the node
	@param			allow_implicit		A boolean: `TRUE` if keys without a
										delimiter are allowed, `FALSE` otherwise
	@param			format				The format of the INI file
	@return			The node type (see header)

**/
static uint8_t get_type_as_active (
	const char * const nodestr,
	const size_t len,
	const _LIBCONFINI_BOOL_ allow_implicit,
	const IniFormat format
) {

	if (!len || *((unsigned char *) nodestr) == format.delimiter_symbol || is_comment_char(*nodestr, format)) {

		return INI_UNKNOWN;

	}

	register uint16_t abcd;
	size_t idx;

	if (format.section_paths != INI_NO_SECTIONS && *nodestr == _LIBCONFINI_OPEN_SECTION_) {

		if (format.no_spaces_in_names) {

			/*

				Search of the CLOSE_SECTION character and possible spaces in
				names -- i.e., ECMAScript `/[^\.\s]\s+[^\.\s]/g.test(nodestr)`.
				The algorithm is made more complex by the fact that LF and CR
				characters are still escaped at this stage.

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
				FLAG_128	We are inside a section name
				FLAG_256	Continue the loop
				FLAG_512	Section path is valid

			*/

			/* Revision #1 */

			for (

				idx = 1,
				abcd	=	(format.section_paths == INI_ONE_LEVEL_ONLY ? 260: 256) |
							(format.no_double_quotes << 1) |
							format.no_single_quotes;

					abcd & 256;

				abcd	=	idx >= len ?
								abcd & 767
							: !(abcd & 42) && nodestr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								(abcd & 991) ^ 16
							: !(abcd & 49) && nodestr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								(abcd & 991) ^ 8
							: nodestr[idx] == _LIBCONFINI_LF_ || nodestr[idx] == _LIBCONFINI_CR_ ?
								(abcd & 991) | 64
							: is_some_space(nodestr[idx], _LIBCONFINI_NO_EOL_) ?
								(
									abcd & 32 ?
										(abcd & 767) | 64
									:
										abcd | 64
								)
							: !(abcd & 28) && nodestr[idx] == _LIBCONFINI_SUBSECTION_ ?
								(
									~abcd & 224 ?
										abcd & 831
									:
										abcd & 767
								)
							: !(abcd & 24) && nodestr[idx] == _LIBCONFINI_CLOSE_SECTION_ ?
								(
									~abcd & 224 ?
										(abcd & 671) | 512
									:
										abcd & 767
								)
							: nodestr[idx] != _LIBCONFINI_BACKSLASH_ ?
								(
									~abcd & 192 ?
										(abcd & 927) | 128
									:
										(abcd & 671) | 128
								)
							: !(abcd & 32) ?
								abcd | 32
							: abcd & 64 ?
								(abcd & 735) | 128
							:
								(abcd & 991) | 128,

				idx++

			);

			return abcd & 512 ? INI_SECTION : INI_UNKNOWN;

		} else {

			/*

			Mask `abcd` (5 bits used):

				FLAG_1		Single quotes are not metacharacters (const)
				FLAG_2		Double quotes are not metacharacters (const)
				FLAG_4		Unescaped single quotes are odd right now
				FLAG_8		Unescaped double quotes are odd right now
				FLAG_16		We are in an odd sequence of backslashes

			*/

			for (abcd = (format.no_double_quotes << 1) | format.no_single_quotes, idx = 1; idx < len; idx++) {

				if (!(abcd & 12) && nodestr[idx] == _LIBCONFINI_CLOSE_SECTION_) {

					return INI_SECTION;

				}

				abcd	=	nodestr[idx] == _LIBCONFINI_BACKSLASH_ ? abcd ^ 16
							: !(abcd & 22) && nodestr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abcd ^ 8
							: !(abcd & 25) && nodestr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abcd ^ 4
							: abcd & 15;

			}

		}

		return INI_UNKNOWN;

	}

	/* It can be just a key... */

	/*

	Mask `abcd` (2 bits used):

		FLAG_1		The delimiter **must** be present
		FLAG_2		Search for spaces in names

	*/

	abcd = (format.no_spaces_in_names && format.delimiter_symbol ? 2 : 0) | (allow_implicit ? 0 : 1);

	if (abcd) {

		idx = get_delimiter_pos(nodestr, len, format);

		if ((abcd & 1) && idx == len) {

			return INI_UNKNOWN;

		}

		if (abcd & 2) {

			idx = urtrim_s(nodestr, idx);

			do {

				if (is_some_space(nodestr[--idx], _LIBCONFINI_WITH_EOL_)) {

					return INI_UNKNOWN;

				}

			} while (idx);

		}

	}

	return INI_KEY;

}


/**

	@brief			Examines a (single-/multi- line) segment and checks whether
					it contains sub-segments.
	@param			segment		Segment to examine
	@param			format		The format of the INI file
	@return			Number of segments found

**/
static size_t further_cuts (char * const segment, const IniFormat format) {

	_LIBCONFINI_BOOL_ forget_me;
	register uint8_t abcd = (format.no_double_quotes << 1) | format.no_single_quotes;
	size_t idx, focus_at, unparsable_at, search_at = 0, segm_size = 0;

	/* ====> */   search_for_cuts:   /* <==== */

	if (!segment[search_at]) {

		goto no_more_cuts;

	}

	unparsable_at = 0;
	forget_me = is_forget_char(segment[search_at], format);

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

	*/

	abcd	=	is_parsable_char(segment[search_at], format) && !(
					format.no_disabled_after_space && is_some_space(segment[search_at + 1], _LIBCONFINI_NO_EOL_)
				) ?
					(abcd & 3) | 64
				: format.multiline_nodes == INI_MULTILINE_EVERYWHERE && (
					segment[search_at] == _LIBCONFINI_INLINE_MARKER_ || is_comment_char(segment[search_at], format)
				) ?
					(abcd & 3) | 128
				:
					abcd & 3;


	if (abcd & 192) {

		for (idx = ltrim_s(segment, search_at + 1, _LIBCONFINI_NO_EOL_); segment[idx]; idx++) {

			if (is_comment_char(segment[idx], format)) {

				/* Search for inline comments in (supposedly) disabled items */

				if (!(abcd & 172)) {

					focus_at = ltrim_s(segment, search_at + 1, _LIBCONFINI_NO_EOL_);

					if (get_type_as_active(segment + focus_at, idx - focus_at, format.disabled_can_be_implicit, format)) {

						if (!is_forget_char(segment[idx], format)) {

							segment[idx] = _LIBCONFINI_INLINE_MARKER_;
							segm_size++;

						}

						segment[idx - 1] = '\0';

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

			} else if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

				/* Search for `/\\(?:\n\r?|\r\n?)\s*[^;#]/` in multi-line disabled items */

				focus_at = ltrim_s(segment, search_at + 1, _LIBCONFINI_NO_EOL_);
				idx = ltrim_s(segment, idx + 1, _LIBCONFINI_WITH_EOL_);

				if (
					forget_me ?
						!is_forget_char(segment[idx], format)
					:
						is_forget_char(segment[idx], format) || !(
							(
								format.multiline_nodes < 2 && is_parsable_char(segment[idx], format) && !(
									format.no_disabled_after_space && (abcd & 64) && is_some_space(segment[idx + 1], _LIBCONFINI_NO_EOL_)
								)
							) || (
								format.multiline_nodes == INI_MULTILINE_EVERYWHERE && is_comment_char(segment[idx], format) && !(
									(abcd & 64) && get_type_as_active(
										segment + focus_at,
										idx - focus_at,
										format.disabled_can_be_implicit,
										format
									)
								)
							)
						)
				) {

					rtrim_h(segment, idx, _LIBCONFINI_WITH_EOL_);
					search_at = qultrim_h(segment, idx, format);
					goto search_for_cuts;

				}

				abcd &= 207;

			} else if (is_some_space(segment[idx], _LIBCONFINI_NO_EOL_)) {

				abcd &= 207;

			} else {

				abcd	=	segment[idx] == _LIBCONFINI_BACKSLASH_ ?
								(abcd | 32) ^ 16
							: !(abcd & 22) && segment[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								(abcd | 32) ^ 8
							: !(abcd & 25) && segment[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								(abcd | 32) ^ 4
							:
								(abcd & 239) | 32;

			}

		}

		if (format.multiline_nodes != INI_MULTILINE_EVERYWHERE && !unparsable_at) {

			focus_at = ltrim_s(segment, search_at + 1, _LIBCONFINI_NO_EOL_);

			if (segment[focus_at] && !get_type_as_active(segment + focus_at, idx - focus_at, format.disabled_can_be_implicit, format)) {

				unparsable_at = search_at + 1;

			}

		}

	} else if (is_comment_char(segment[search_at], format)) {

		unparsable_at = search_at + 1;

	} else {

		/* Search for inline comments in active items */

		/*

		Mask `abcd` (7 bits used):

			FLAG_1		Single quotes are not metacharacters (const)
			FLAG_2		Double quotes are not metacharacters (const)
			FLAG_4		Unescaped single quotes are odd right now
			FLAG_8		Unescaped double quotes are odd right now
			FLAG_16		We are in an odd sequence of backslashes
			FLAG_32		This is not a hash nor a semicolon character
			FLAG_64		The previous character is not a space

		*/

		for (abcd = (abcd & 3) | 96, idx = search_at + 1; segment[idx]; idx++) {

			abcd	=	is_comment_char(segment[idx], format) ?
							abcd & 79
						: is_some_space(segment[idx], _LIBCONFINI_WITH_EOL_) ?
							(abcd & 47) | 32
						: segment[idx] == _LIBCONFINI_BACKSLASH_ ?
							(abcd | 96) ^ 16
						: !(abcd & 22) && segment[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
							(abcd | 96) ^ 8
						: !(abcd & 25) && segment[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
							(abcd | 96) ^ 4
						:
							(abcd & 111) | 96;


			if (!(abcd & 124)) {

				segment[idx - 1] = '\0';

				if (!is_forget_char(segment[idx], format)) {

					segment[idx] = _LIBCONFINI_INLINE_MARKER_;

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

		/* Cut unparsable multi-line comments */

		for (idx = unparsable_at; segment[idx]; idx++) {

			if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

				search_at = qultrim_h(segment, idx, format);
				goto search_for_cuts;

			}

		}

	}

	/* ====> */   no_more_cuts:   /* <==== */

	return segm_size;

}


	/******************************************************************************/



		/*\
		|*|
		|*|   GLOBAL ENVIRONMENT
		|*|
		\*/



		/* LIBRARY'S MAIN FUNCTIONS */


													/** @utility{load_ini_file} **/
/**

	@brief			Parses an INI file and dispatches its content
	@param			ini_file		The `FILE` structure pointing to the INI file
									to parse
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the
									dispatch, or NULL
	@param			f_foreach		The function that will be invoked for each
									dispatch, or NULL
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

	The function @p f_init will be invoked with two arguments: `statistics` (a
	pointer to an `IniStatistics` object containing some properties about the file
	read) and `init_other` (the custom argument @p user_data previously passed). If
	@p f_init returns a non-zero value the caller function will be interrupted.

	The function @p f_foreach will be invoked with two arguments: `dispatch` (a
	pointer to an `IniDispatch` object containing the parsed member of the INI file)
	and `foreach_other` (the custom argument @p user_data previously passed). If
	@p f_foreach returns a non-zero value the caller function will be interrupted.

	@include topics/load_ini_file.c

**/
int load_ini_file (
	FILE * const ini_file,
	const IniFormat format,
	int (* const f_init) (
		IniStatistics *statistics,
		void *init_other
	),
	int (* const f_foreach) (
		IniDispatch *dispatch,
		void *foreach_other
	),
	void *user_data
) {

	fseek(ini_file, 0, SEEK_END);

	#define __N_BYTES__ tmp_size_1

	size_t tmp_size_1 = ftell(ini_file);
	char * const cache = (char *) malloc(__N_BYTES__ + 1);

	if (cache == NULL) {

		return CONFINI_ENOMEM;

	}

	int return_value = _LIBCONFINI_SUCCESS_;

	rewind(ini_file);

	if (fread(cache, 1, __N_BYTES__, ini_file) < __N_BYTES__) {

		return_value = CONFINI_EIO;
		goto free_and_exit;

	}

	cache[__N_BYTES__] = '\0';

	_LIBCONFINI_BOOL_ tmp_bool;
	register uint16_t abcd;
	size_t idx, node_at, tmp_size_2, tmp_size_3;

	/* PART ONE: Examine and isolate each segment */

	#define __ISNT_ESCAPED__ tmp_bool
	#define __EOL_ID__ abcd
	#define __N_MEMBERS__ tmp_size_2
	#define __SHIFT_LEN__ tmp_size_3

	/* UTF-8 BOM */

	__SHIFT_LEN__ = *((unsigned char *) cache) == 0xEF && *((unsigned char *) cache + 1) == 0xBB && *((unsigned char *) cache + 2) == 0xBF ? 3 : 0;

	for (

		__N_MEMBERS__ = 0,
		__EOL_ID__ = _LIBCONFINI_EOL_IDX_,
		__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_,
		node_at = 0,
		idx = __SHIFT_LEN__;

			idx < __N_BYTES__;

		idx++

	) {

		cache[idx - __SHIFT_LEN__] = cache[idx];

		if (cache[idx] == _LIBCONFINI_SPACES_[__EOL_ID__] || cache[idx] == _LIBCONFINI_SPACES_[__EOL_ID__ ^= 1]) {

			if (format.multiline_nodes == INI_NO_MULTILINE || __ISNT_ESCAPED__) {

				cache[idx - __SHIFT_LEN__] = '\0';
				__N_MEMBERS__ += further_cuts(cache + qultrim_h(cache, node_at, format), format);
				node_at = idx - __SHIFT_LEN__ + 1;

			} else if (cache[idx + 1] == _LIBCONFINI_SPACES_[__EOL_ID__ ^ 1]) {

				idx++;

			}

			__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_;

		} else if (cache[idx] == _LIBCONFINI_BACKSLASH_) {

			__ISNT_ESCAPED__ = !__ISNT_ESCAPED__;

		} else if (cache[idx]) {

			__ISNT_ESCAPED__ = _LIBCONFINI_TRUE_;

		} else {

			/* Remove `NUL` characters in the buffer (if any) */

			__SHIFT_LEN__++;

		}

	}

	const size_t len = idx - __SHIFT_LEN__;

	while (idx > len) {

		cache[--idx] = '\0';

	}

	__N_MEMBERS__ += further_cuts(cache + qultrim_h(cache, node_at, format), format);

	IniStatistics this_doc = {
		.format = format,
		.bytes = __N_BYTES__,
		.members = __N_MEMBERS__
	};

	#undef __SHIFT_LEN__
	#undef __N_MEMBERS__
	#undef __EOL_ID__
	#undef __ISNT_ESCAPED__
	#undef __N_BYTES__

	/* Debug */

	/*

	for (size_t tmp = 0; tmp < len + 1; tmp++) {
		putchar(cache[tmp] == 0 ? '$' : cache[tmp]);
	}
	putchar('\n');

	*/

	if (f_init && f_init(&this_doc, user_data)) {

		return_value = CONFINI_IINTR;
		goto free_and_exit;

	}

	if (!f_foreach) {

		goto free_and_exit;

	}

	/* PART TWO: Dispatch the parsed input */

	#define __PARENT_IS_DISABLED__ tmp_bool
	#define __REAL_PARENT_LEN__ tmp_size_1
	#define __CURR_PARENT_LEN__ tmp_size_2
	#define __ITER__ tmp_size_3

	__REAL_PARENT_LEN__ = 0, __CURR_PARENT_LEN__ = 0;

	size_t parse_at;
	char *curr_parent_str = cache + len, *subparent_str = curr_parent_str, *real_parent_str = curr_parent_str;

	IniDispatch this_disp = {
		.format = format,
		.dispatch_id = 0
	};

	__PARENT_IS_DISABLED__ = _LIBCONFINI_FALSE_;

	for (node_at = 0, idx = 0; idx <= len; idx++) {

		if (cache[idx]) {

			continue;

		}

		if (!cache[node_at] || is_forget_char(cache[node_at], format)) {

			node_at = idx + 1;
			continue;

		}

		if (this_disp.dispatch_id >= this_doc.members) {

			return_value = CONFINI_EFEOOR;
			goto free_and_exit;

		}

		this_disp.data = cache + node_at;
		this_disp.value = cache + idx;
		this_disp.type = 0;
		this_disp.d_len = idx - node_at;
		this_disp.v_len = 0;

		if (is_parsable_char(*this_disp.data, format)) {

			parse_at = ltrim_s(cache, node_at + 1, _LIBCONFINI_NO_EOL_);

			/*

				Search for inline comments left unmarked *inside* a parsable comment:
				if found it means that the comment is not parsable.

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
				FLAG_256	Last was not `\s[;#]`, or was it but was semantic
							rather than syntactic
				FLAG_512	Continue the loop

			*/

			abcd	=	(format.no_disabled_after_space ? 512 : 516) |
						(format.no_double_quotes << 1) |
						format.no_single_quotes;

			for (__ITER__ = 1; (abcd & 512) && __ITER__ < this_disp.d_len; __ITER__++) {

				abcd	=	this_disp.data[__ITER__] == _LIBCONFINI_LF_ || this_disp.data[__ITER__] == _LIBCONFINI_CR_ ?
								abcd | 448
							: is_comment_char(this_disp.data[__ITER__], format) ?
								(
									(abcd & 64) && !is_parsable_char(this_disp.data[__ITER__], format) ?
										abcd & 31
									: abcd & 128 ?
										abcd & 543
									: abcd & 24 ?
										(abcd & 799) | 256
									:
										abcd & 287
								)
							: is_some_space(this_disp.data[__ITER__], _LIBCONFINI_NO_EOL_) ?
								(
									abcd & 64 ?
										(abcd & 991) | 448
									: abcd & 260 ?
										(abcd & 799) | 256
									:
										abcd & 31
								)
							: this_disp.data[__ITER__] == _LIBCONFINI_BACKSLASH_ ?
								((abcd & 831) | 256) ^ 32
							: !(abcd & 42) && this_disp.data[__ITER__] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abcd & 799) | 256) ^ 16
							: !(abcd & 49) && this_disp.data[__ITER__] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abcd & 799) | 256) ^ 8
							:
								(abcd & 799) | 256;

			}

			this_disp.type		=	__ITER__ == this_disp.d_len ?
										get_type_as_active(cache + parse_at, idx - parse_at, format.disabled_can_be_implicit, format)
									:
										0;

			if (this_disp.type) {

				this_disp.data = cache + parse_at;
				this_disp.d_len = idx - parse_at;

			}

			this_disp.type |= 4;

		} else if (is_comment_char(*this_disp.data, format)) {

			this_disp.type = INI_COMMENT;

		} else if (*this_disp.data == _LIBCONFINI_INLINE_MARKER_) {

			this_disp.type = INI_INLINE_COMMENT;

		} else {

			this_disp.type = get_type_as_active(this_disp.data, this_disp.d_len, _LIBCONFINI_TRUE_, format);

		}

		if (__CURR_PARENT_LEN__ && *subparent_str) {

			__ITER__ = 0;

			do {

				curr_parent_str[__ITER__ + __CURR_PARENT_LEN__] = subparent_str[__ITER__];

			} while (subparent_str[__ITER__++]);

			__CURR_PARENT_LEN__ += __ITER__ - 1;
			subparent_str = curr_parent_str + __CURR_PARENT_LEN__;

		}

		if (__PARENT_IS_DISABLED__ && !(this_disp.type & 4)) {

			real_parent_str[__REAL_PARENT_LEN__] = '\0';
			__CURR_PARENT_LEN__ = __REAL_PARENT_LEN__;
			curr_parent_str = real_parent_str;
			__PARENT_IS_DISABLED__ = _LIBCONFINI_FALSE_;

		} else if (!__PARENT_IS_DISABLED__ && this_disp.type == INI_DISABLED_SECTION) {

			__REAL_PARENT_LEN__ = __CURR_PARENT_LEN__;
			real_parent_str = curr_parent_str;
			__PARENT_IS_DISABLED__ = _LIBCONFINI_TRUE_;

		}

		this_disp.append_to = curr_parent_str;
		this_disp.at_len = __CURR_PARENT_LEN__;

		this_disp.d_len		=	this_disp.type == INI_COMMENT ? 
									uncomment(this_disp.data, idx - node_at, format)
								: this_disp.type == INI_INLINE_COMMENT ? 
									uncomment(++this_disp.data, idx - node_at - 1, format)
								: format.multiline_nodes != INI_NO_MULTILINE ?
									unescape_cr_lf(this_disp.data, idx - node_at, this_disp.type & 4, format)
								:
									idx - node_at;

		switch (this_disp.type) {

			/*
			case INI_UNKNOWN:
			*/

				/* Do nothing */

				/*
				break;
				*/

			case INI_SECTION:
			case INI_DISABLED_SECTION:

				*this_disp.data++ = '\0';

				/*

				Mask `abcd` (5 bits used):

					FLAG_1		Single quotes are not metacharacters (const)
					FLAG_2		Double quotes are not metacharacters (const)
					FLAG_4		Unescaped single quotes are odd right now
					FLAG_8		Unescaped double quotes are odd right now
					FLAG_16		We are in an odd sequence of backslashes

				*/

				for (

					abcd = (format.no_double_quotes << 1) | format.no_single_quotes, __ITER__ = 0;

						this_disp.data[__ITER__] && (
							(abcd & 12) || this_disp.data[__ITER__] != _LIBCONFINI_CLOSE_SECTION_
						);

					abcd	=	this_disp.data[__ITER__] == _LIBCONFINI_BACKSLASH_ ?
									abcd ^ 16
								: !(abcd & 22) && this_disp.data[__ITER__] == _LIBCONFINI_DOUBLE_QUOTES_ ?
									abcd ^ 8
								: !(abcd & 25) && this_disp.data[__ITER__] == _LIBCONFINI_SINGLE_QUOTES_ ?
									abcd ^ 4
								:
									abcd & 15,

					__ITER__++

				);

				while (this_disp.data[__ITER__]) {

					this_disp.data[__ITER__++] = '\0';

				}

				this_disp.d_len		=	format.section_paths == INI_ONE_LEVEL_ONLY ?
											sanitize_key_name(this_disp.data, format)
										:
											sanitize_section_path(this_disp.data, format);


				if (format.section_paths == INI_ONE_LEVEL_ONLY || *this_disp.data != _LIBCONFINI_SUBSECTION_) {

					/* Append to root (it is an absolute path) */
					curr_parent_str = this_disp.data;
					__CURR_PARENT_LEN__ = this_disp.d_len;
					subparent_str = cache + idx;
					this_disp.append_to = subparent_str;
					this_disp.at_len = 0;

				} else if (format.section_paths == INI_ABSOLUTE_ONLY || !__CURR_PARENT_LEN__) {

					/* Append to root and remove the leading dot (it is a relative path, but parent is root or relative paths are not allowed) */
					curr_parent_str = ++this_disp.data;
					__CURR_PARENT_LEN__ = --this_disp.d_len;
					subparent_str = cache + idx;
					this_disp.append_to = subparent_str;
					this_disp.at_len = 0;

				} else if (this_disp.d_len != 1) {

					/* Append to the current parent (it is a relative path and parent is not root) */
					subparent_str = this_disp.data;

				}

				if (INI_GLOBAL_LOWERCASE_MODE && !format.case_sensitive) {

					string_tolower(this_disp.data);

				}

				break;

			case INI_KEY:
			case INI_DISABLED_KEY:

				__ITER__ = get_delimiter_pos(this_disp.data, this_disp.d_len, format);

				if (this_disp.d_len && __ITER__ && __ITER__ < this_disp.d_len) {

					this_disp.data[__ITER__] = '\0';

					if (format.do_not_collapse_values) {

						this_disp.value = this_disp.data + ltrim_h(this_disp.data, __ITER__ + 1, _LIBCONFINI_WITH_EOL_);
						this_disp.v_len = rtrim_h(this_disp.value, this_disp.d_len + this_disp.data - this_disp.value, _LIBCONFINI_WITH_EOL_);

					} else if (format.preserve_empty_quotes) {

						this_disp.value = this_disp.data + __ITER__ + 1;
						this_disp.v_len = collapse_spaces(this_disp.value, format);

					} else {

						this_disp.value = this_disp.data + __ITER__ + 1;
						this_disp.v_len = sanitize_key_name(this_disp.value, format);

					}

				} else if (format.implicit_is_not_empty) {

					this_disp.value = INI_GLOBAL_IMPLICIT_VALUE;
					this_disp.v_len = INI_GLOBAL_IMPLICIT_V_LEN;
				}

				this_disp.d_len = sanitize_key_name(this_disp.data, format);

				if (INI_GLOBAL_LOWERCASE_MODE && !format.case_sensitive) {

					string_tolower(this_disp.data);

				}

				break;

			case INI_COMMENT:
			case INI_INLINE_COMMENT:

				this_disp.append_to = cache + idx;
				this_disp.at_len = 0;

		}

		if (f_foreach(&this_disp, user_data)) {

			return_value = CONFINI_FEINTR;
			goto free_and_exit;

		}

		this_disp.dispatch_id++;
		node_at = idx + 1;

	}

	/* ====> */   free_and_exit:   /* <==== */

	#undef __ITER__
	#undef __CURR_PARENT_LEN__
	#undef __REAL_PARENT_LEN__
	#undef __PARENT_IS_DISABLED__

	free(cache);

	return return_value;


}


													/** @utility{load_ini_path} **/
/**

	@brief			Parses an INI file and dispatches its content
	@param			path			The path of the INI file
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the
									dispatch, or `NULL`
	@param			f_foreach		The function that will be invoked for each
									dispatch, or `NULL`
	@param			user_data		A custom argument, or `NULL`
	@return			Zero for success, otherwise an error code

	For the two parameters @p f_init and @p f_foreach see function `load_ini_file()`.

	@include topics/load_ini_path.c

**/
int load_ini_path (
	const char * const path,
	const IniFormat format,
	int (* const f_init) (
		IniStatistics *statistics,
		void *init_other
	),
	int (* const f_foreach) (
		IniDispatch *dispatch,
		void *foreach_other
	),
	void *user_data
) {

	FILE * const ini_file = fopen(path, "r");

	if (ini_file == NULL) {

		return CONFINI_ENOENT;

	}

	int return_value = load_ini_file(ini_file, format, f_init, f_foreach, user_data);

	fclose(ini_file);

	return return_value;

}



		/* OTHER UTILITIES FOR THE USER (NOT USED BY LIBCONFINI) */


									/** @utility{ini_global_set_lowercase_mode} **/
/**

	@brief			Sets the value of the global variable
					`#INI_GLOBAL_LOWERCASE_MODE`
	@param			lowercase				The new value 
	@return			Nothing

	If @p lowercase is `TRUE` key and section names in case-insensitive INI formats
	will be dispatched lowercase, verbatim otherwise (default value: `TRUE`).

**/
void ini_global_set_lowercase_mode (_Bool lowercase) {

	INI_GLOBAL_LOWERCASE_MODE = lowercase;

}


									/** @utility{ini_global_set_implicit_value} **/
/**

	@brief			Sets the value used for implicit keys
	@param			implicit_value		The string to be used as implicit value
										(usually `"YES"`, or `"TRUE"`)
	@param			implicit_v_len		The length of @p implicit_value (usually
										`0`, independently of its real length)
	@return			Nothing

	In order to be thread-safe this function should be used only once, or otherwise
	a mutex must be introduced.

	@include topics/ini_global_set_implicit_value.c

**/
void ini_global_set_implicit_value (char * const implicit_value, const size_t implicit_v_len) {

	INI_GLOBAL_IMPLICIT_VALUE = implicit_value;
	INI_GLOBAL_IMPLICIT_V_LEN = implicit_v_len;

}


														/** @utility{ini_fton} **/
/**

	@brief			Converts an `IniFormat` into an `::IniFormatNum`
	@param			source			The IniFormat to be read
	@return			The mask representing the format

**/
IniFormatNum ini_fton (const IniFormat source) {

	uint8_t bitpos = 0;
	IniFormatNum mask = 0;

	#define __CALC_FORMAT_ID__(SIZE, PROPERTY, IGNORE_ME) \
		mask |= source.PROPERTY << bitpos;\
		bitpos += SIZE;

	_LIBCONFINI_INIFORMAT_AS_(__CALC_FORMAT_ID__)

	#undef __CALC_FORMAT_ID__

	return mask;

}


														/** @utility{ini_ntof} **/
/**

	@brief			Constructs an `IniFormat` from an `::IniFormatNum`
	@param			dest_format			The IniFormat to be set
	@param			mask				The `#IniFormatNum` to be read
	@return			The `IniFormat` constructed

**/
IniFormat ini_ntof (IniFormatNum format_id) {

	IniFormat dest_format;

	#define __MAX_1_BITS__ 1
	#define __MAX_2_BITS__ 3
	#define __MAX_3_BITS__ 7
	#define __MAX_4_BITS__ 15
	#define __MAX_5_BITS__ 31
	#define __MAX_6_BITS__ 63
	#define __MAX_7_BITS__ 127
	#define __MAX_8_BITS__ 255
	#define __READ_FORMAT_ID__(SIZE, PROPERTY, IGNORE_ME) \
		dest_format.PROPERTY = format_id & __MAX_##SIZE##_BITS__;\
		format_id >>= SIZE;

	_LIBCONFINI_INIFORMAT_AS_(__READ_FORMAT_ID__)

	#undef __READ_FORMAT_ID__
	#undef __MAX_8_BITS__
	#undef __MAX_7_BITS__
	#undef __MAX_6_BITS__
	#undef __MAX_5_BITS__
	#undef __MAX_4_BITS__
	#undef __MAX_3_BITS__
	#undef __MAX_2_BITS__
	#undef __MAX_1_BITS__

	return dest_format;

}


											/** @utility{ini_string_match_ss} **/
/**

	@brief			Compares two simple strings and checks if they match
	@param			simple_string_a		The first simple string
	@param			simple_string_b		The second simple string
	@return			A boolean: `TRUE` if the two strings match, `FALSE` otherwise

	Simple strings are user-given strings or the result of `ini_string_parse()`. The
	@p format argument is used for the following fields:

	- `format.case_sensitive`

**/
_Bool ini_string_match_ss (const char * const simple_string_a, const char * const simple_string_b, const IniFormat format) {

	_Bool they_match = _LIBCONFINI_TRUE_;
	size_t idx = 0;

	if (format.case_sensitive) {

		do {

			they_match = simple_string_a[idx] == simple_string_b[idx];

		} while (they_match && simple_string_a[idx++]);

	} else {

		do {

			they_match = _LIBCONFINI_CHR_CASEFOLD_(simple_string_a[idx]) == _LIBCONFINI_CHR_CASEFOLD_(simple_string_b[idx]);

		} while (they_match && simple_string_a[idx++]);

	}

	return they_match;

}


											/** @utility{ini_string_match_si} **/
/**

	@brief			Compares an INI string with a simple string and checks if they
					match according to a format
	@param			ini_string			The INI string escaped according to
										@p format
	@param			simple_string		The simple string
	@param			format				The format of the INI file
	@return			A boolean: `TRUE` if the two strings match, `FALSE` otherwise

	INI strings are the strings typically dispatched by `load_ini_file()` and
	`load_ini_path()`, which may contain quotes and the three escaping sequences
	`\\`, `\'` and `\"`. Simple strings are user-given strings or the result of
	`ini_string_parse()`.

	The @p format argument is used for the following fields:

	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes`
	- `format.case_sensitive`

	@include topics/ini_string_match_si.c

**/
_Bool ini_string_match_si (const char * const simple_string, const char * const ini_string, const IniFormat format) {

	if (format.no_double_quotes && format.no_single_quotes && format.multiline_nodes == INI_NO_MULTILINE) {

		/* There are no escape sequences. I will perform a simple comparison between strings. */

		return ini_string_match_ss(simple_string, ini_string, format);

	}

	_Bool they_match = _LIBCONFINI_TRUE_;
	register uint8_t abcd;
	size_t idx_i = 0, idx_s = 0, nbacksl = 0;

	abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

	/*

	Mask `abcd` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		This is an unescaped single quote and format supports single
					quotes
		FLAG_32		This is an unescaped double quote and format supports double
					quotes
		FLAG_64		This is a space
		FLAG_128	Skip this character

	*/

	do {

		/* ====> */   skip_character:   /* <==== */

		while (ini_string[idx_i] == _LIBCONFINI_BACKSLASH_) {

			nbacksl++;
			idx_i++;

		}

		abcd	=	!(abcd & 6) && ini_string[idx_i] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(nbacksl & 1 ? (abcd & 47) | 32 : ((abcd & 239) | 160) ^ 8)
					: !(abcd & 9) && ini_string[idx_i] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(nbacksl & 1 ? (abcd & 31) | 16 : ((abcd & 223) | 144) ^ 4)
					: (abcd & 12) || !is_some_space(ini_string[idx_i], _LIBCONFINI_WITH_EOL_) ?
						abcd & 15
					: abcd & 64 ?
						(abcd & 207) | 128
					:
						(abcd & 15) | 64;


		if (abcd & 128) {

			idx_i++;
			goto skip_character;

		}

		if (!(abcd & 48)) {

			nbacksl++;

		}

		nbacksl >>= 1;

		for (

			nbacksl++;

				--nbacksl && they_match;

			they_match = simple_string[idx_s] && simple_string[idx_s++] == _LIBCONFINI_BACKSLASH_

		);

		if (
			they_match && (
				abcd & 64 ?
					is_some_space(simple_string[idx_s], _LIBCONFINI_WITH_EOL_)
				: format.case_sensitive ?
					ini_string[idx_i] == simple_string[idx_s]
				:
					_LIBCONFINI_CHR_CASEFOLD_(ini_string[idx_i]) == _LIBCONFINI_CHR_CASEFOLD_(simple_string[idx_s])
			)
		) {

			nbacksl = 0;
			idx_s++;

		} else {

			they_match = _LIBCONFINI_FALSE_;

		}

	} while (they_match && ini_string[idx_i++]);

	return they_match;

}


											/** @utility{ini_string_match_ii} **/
/**

	@brief			Compares two INI strings and checks if they match according to a
					format
	@param			ini_string_a		The first INI string unescaped according to
										@p format
	@param			ini_string_b		The second INI string unescaped according to
										@p format
	@param			format				The format of the INI file
	@return			A boolean: `TRUE` if the two strings match, `FALSE` otherwise

	INI strings are the strings typically dispatched by `load_ini_file()` and
	`load_ini_path()`, which may contain quotes and the three escaping sequences
	`\\`, `\'` and `\"`.

	The @p format argument is used for the following fields:

	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_nodes`
	- `format.case_sensitive`

**/
_Bool ini_string_match_ii (const char * const ini_string_a, const char * const ini_string_b, const IniFormat format) {

	if (format.no_double_quotes && format.no_single_quotes && format.multiline_nodes == INI_NO_MULTILINE) {

		/* There are no escape sequences. I will perform a simple comparison between strings. */

		return ini_string_match_ss(ini_string_a, ini_string_b, format);

	}

	_Bool they_match = _LIBCONFINI_TRUE_;
	uint8_t side = 0, a_abcd[2];
	const char * const a_str[2] = { ini_string_a, ini_string_b };
	size_t a_idx[2] = { 0, 0 }, a_nbacksl[2] = { 0, 0 };

	a_abcd[1] = *a_abcd = (format.no_double_quotes << 1) | format.no_single_quotes;

	/*

	Masks `a_abcd[0]` and  `a_abcd[1]` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd right now
		FLAG_8		Unescaped double quotes are odd right now
		FLAG_16		This is an unescaped single quote and format supports single
					quotes
		FLAG_32		This is an unescaped double quote and format supports double
					quotes
		FLAG_64		This is a space
		FLAG_128	Skip this character

	*/

	do {

		/* ====> */   change_side:   /* <==== */

		side ^= 1;

		/* ====> */   skip_character:   /* <==== */

		while (a_str[side][a_idx[side]] == _LIBCONFINI_BACKSLASH_) {

			a_nbacksl[side]++;
			a_idx[side]++;

		}

		a_abcd[side]	=	!(a_abcd[side] & 6) && a_str[side][a_idx[side]] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								(a_nbacksl[side] & 1 ? (a_abcd[side] & 47) | 32 : ((a_abcd[side] & 239) | 160) ^ 8)
							: !(a_abcd[side] & 9) && a_str[side][a_idx[side]] == _LIBCONFINI_SINGLE_QUOTES_ ?
								(a_nbacksl[side] & 1 ? (a_abcd[side] & 31) | 16 : ((a_abcd[side] & 223) | 144) ^ 4)
							: (a_abcd[side] & 12) || !is_some_space(a_str[side][a_idx[side]], _LIBCONFINI_WITH_EOL_) ?
								a_abcd[side] & 15
							: a_abcd[side] & 64 ?
								(a_abcd[side] & 207) | 128
							:
								(a_abcd[side] & 15) | 64;


		if (a_abcd[side] & 128) {

			a_idx[side]++;
			goto skip_character;

		}

		if (!(a_abcd[side] & 48)) {

			a_nbacksl[side]++;

		}

		if (side) {

			goto change_side;

		}

		if (
			a_nbacksl[1] >> 1 == *a_nbacksl >> 1 && (
				*a_abcd & 64 ?
					(a_abcd[1] & 64) || is_some_space(ini_string_b[a_idx[1]], _LIBCONFINI_WITH_EOL_)
				: a_abcd[1] & 64 ?
					is_some_space(ini_string_a[*a_idx], _LIBCONFINI_WITH_EOL_)
				: format.case_sensitive ?
					ini_string_a[*a_idx] == ini_string_b[a_idx[1]]
				:
					_LIBCONFINI_CHR_CASEFOLD_(ini_string_a[*a_idx]) == _LIBCONFINI_CHR_CASEFOLD_(ini_string_b[a_idx[1]])
			)
		) {

			a_nbacksl[1] = *a_nbacksl = 0;
			a_idx[1]++;

		} else {

			they_match = _LIBCONFINI_FALSE_;

		}

	} while (they_match && ini_string_a[(*a_idx)++]);

	return they_match;

}


													/** @utility{ini_unquote} **/
/**

	@brief			Unescapes `\\`, `\'` and `\"` and removes all unescaped quotes
					(if single/double quotes are considered metacharacters in
					respect to the format given)
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

	This function is very similar to `ini_string_parse()`, except that does not
	collapse the spaces surrounding empty quotes after these have been removed --
	key names dispatched by **libconfini** are _always_ collapsed strings.

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be
	used as well). If the string does not contain quotes, or if quotes are
	considered to be normal characters, no changes will be made.

	The @p format argument is used for the following fields:

	- `format.multiline_nodes`
	- `format.no_single_quotes`
	- `format.no_double_quotes`

	@include topics/ini_string_parse.c

**/
size_t ini_unquote (char * const ini_string, const IniFormat format) {

	size_t idx = 0;

	if (format.no_double_quotes && format.no_single_quotes && format.multiline_nodes == INI_NO_MULTILINE) {

		/* There are no escape sequences. I will just return the length of the string. */

		for (; ini_string[idx]; idx++);

		return idx;

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


												/** @utility{ini_string_parse} **/
/**

	@brief			Unescapes `\\`, `\'` and `\"` and removes all unescaped quotes
					(if single/double quotes are considered metacharacters in
					respect to the format given)
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be
	used as well). If `format.do_not_collapse_values` is set to non-zero, spaces
	surrounding empty quotes will be collapsed together with the latter.

	The @p format argument is used for the following fields:

	- `format.multiline_nodes`
	- `format.no_single_quotes`
	- `format.no_double_quotes`
	- `format.do_not_collapse_values`

	Note:	`format.multiline_nodes` is used only to figure out whether there are
			escape sequences or not. For all other purposes new line characters will
			be considered to be equal to any other spaces, even if the format is not
			multi-line -- new line characters in non-multi-line formats should never
			appear (unless you wrote yourself the INI string to parse).

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

	size_t idx = 0, lshift = 0;
	register uint8_t abcd	=	(format.do_not_collapse_values ? 68 : 64) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;


	if (abcd == 67 && format.multiline_nodes == INI_NO_MULTILINE) {

		/* There are no escape sequences but spaces must still be collapsed. */

		for (; ini_string[idx]; idx++) {

			abcd = !is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_) ? 3 : abcd & 64 ? 195 : 67;

			if (abcd & 128) {

				lshift++;

			} else {

				ini_string[idx - lshift] = abcd & 64 ? _LIBCONFINI_SIMPLE_SPACE_ : ini_string[idx];

			}

		}

		for (idx -= (abcd & 64) && idx - lshift ? ++lshift : lshift; ini_string[idx]; ini_string[idx++] = '\0');

		return idx - lshift;

	}

	if (abcd & 4) {

		for (; ini_string[lshift] && is_some_space(ini_string[lshift], _LIBCONFINI_WITH_EOL_); lshift++);

		if (format.no_double_quotes && format.no_single_quotes && format.multiline_nodes == INI_NO_MULTILINE) {

			/* There are no escape sequences and spaces must not be collapsed, but left and right trim might still be necessary. */

			for (; ini_string[idx]; idx++) {

				ini_string[idx - lshift] = ini_string[idx];

			}

			for (idx -= lshift; ini_string[idx]; ini_string[idx++] = '\0');

			return rtrim_h(ini_string, idx - lshift, _LIBCONFINI_WITH_EOL_);

		}

	}

	/* There might be escape sequences */

	size_t nbacksl = 0;

	for (; ini_string[idx]; idx++) {

		abcd	=	!(abcd & 10) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(nbacksl & 1 ? (abcd & 63) | 32 : ((abcd & 223) | 128) ^ 16)
					: !(abcd & 17) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(nbacksl & 1 ? (abcd & 63) | 32 : ((abcd & 223) | 128) ^ 8)
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

		ini_string[idx - lshift] = abcd & 64 ? _LIBCONFINI_SIMPLE_SPACE_ : ini_string[idx];

	}

	lshift += nbacksl >> 1;

	for (idx -= (abcd & 64) && idx - lshift ? ++lshift : lshift; ini_string[idx]; ini_string[idx++] = '\0');

	return (abcd ^ 4) & 28 ? idx - lshift : rtrim_h(ini_string, idx - lshift, _LIBCONFINI_WITH_EOL_);

}


											/** @utility{ini_array_get_length} **/
/**

	@brief			Gets the length of a stringified INI array
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members -- if
									zero (see `#INI_ANY_SPACE`), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The length of the INI array

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be
	used as well).

**/
size_t ini_array_get_length (const char * const ini_string, const char delimiter, const IniFormat format) {

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

	size_t counter = 0;
	register uint8_t abcd	=	(delimiter ? 64 : 68) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;

	for (size_t idx = 0; ini_string[idx]; idx++) {

		/* Revision #1 */

		abcd	=	ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abcd & 63) ^ 32
					: !(abcd & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(abcd & 31) ^ 16
					: !(abcd & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(abcd & 31) ^ 8
					: !(abcd & 28) && ini_string[idx] == delimiter ?
						(abcd & 159) | 128
					: (abcd & 24) || !is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_) ?
						abcd & 31
					: (abcd & 68) ^ 4 ?
						(abcd & 95) | 64
					:
						(abcd & 223) | 192;


		if (abcd & 128) {

			counter++;

		}

	}

	return ~abcd & 68 ? counter + 1 : counter;

}


												/** @utility{ini_array_collapse} **/
/**

	@brief			Compresses the distribution of the data of a stringified INI
					array, by removing all the white spaces that surround its
					delimiters, empty quotes, collapsable spaces, etc.
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members --
									if zero (`INI_ANY_SPACE`) any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The new length of the string containing the array

	Out of quotes similar to ECMAScript
	`ini_string.replace(new RegExp("^\\s+|\\s*(?:(" + delimiter + ")\\s*|($))", "g"), "$1$2")`.
	If `#INI_ANY_SPACE` (`0`) is used as delimiter, one or more different spaces
	(`/[\t \v\f\n\r]+/`) will be always collapsed to one space (`' '`),
	independently of their position.

	Usually @p ini_string comes from an `IniDispatch` (but any other string may	be
	used as well).

	This function can be useful before invoking `memcpy()` using @p ini_string as
	source.

	The @p format argument is used for the following fields:

	- `format.no_double_quotes`
	- `format.no_single_quotes`
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

	@include topics/ini_array_collapse.c

	@note	The actual space occupied by the array might get further reduced after
			each member is parsed by `ini_string_parse()`.

**/
size_t ini_array_collapse (char * const ini_string, const char delimiter, const IniFormat format) {

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
		FLAG_2048	These are some quotes or among the last spaces are some empty quotes
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

		/* Revision #1 */

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
											_LIBCONFINI_SIMPLE_SPACE_;


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


												/** @utility{ini_array_foreach} **/
/**

	@brief			Calls a custom function for each member of a stringified INI
					array without modifying the content of the buffer -- useful for
					read-only (`const`) stringified arrays
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members -- if
									zero (see `#INI_ANY_SPACE`), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@param			f_foreach		The function that will be invoked for each array
									member
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be
	used as well).

	The function @p f_foreach will be invoked with six arguments: `fullstring` (a
	pointer to @p ini_string), `memb_offset` (the offset of the member in bytes),
	`memb_length` (the length of the member in bytes), `memb_num` (the index of the
	member in number of members), `format` (the format of the INI file),
	`foreach_other` (the custom argument @p user_data previously passed). If
	@p f_foreach returns a non-zero value the function `ini_array_foreach()` will be
	interrupted.

**/
int ini_array_foreach (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach) (
		const char *fullstring,
		size_t memb_offset,
		size_t memb_length,
		size_t memb_num,
		IniFormat format,
		void *foreach_other
	),
	void *user_data
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

	register uint8_t abcd	=	(delimiter ? 4 : 0) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;


	for (size_t offs = 0, counter = 0, idx = 0; !(abcd & 128); idx++) {

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

			offs = ltrim_s(ini_string, offs, _LIBCONFINI_WITH_EOL_);

			if (f_foreach(ini_string, offs, rtrim_s(ini_string + offs, idx - offs, _LIBCONFINI_WITH_EOL_), counter++, format, user_data)) {

				return CONFINI_FEINTR;

			}

			if (!(abcd & 132)) {

				idx = ltrim_s(ini_string, idx, _LIBCONFINI_WITH_EOL_) - 1;

			}

			offs = idx + 1;

		}

	}

	return _LIBCONFINI_SUCCESS_;

}


												/** @utility{ini_array_split} **/
/**

	@brief			Splits a stringified INI array into NUL-separated members and
					calls a custom function for each member
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members -- if
									zero (see `#INI_ANY_SPACE`), any space is
									delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@param			f_foreach		The function that will be invoked for each array
									member
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be
	used as well).

	The function @p f_foreach will be invoked with five arguments: `member` (a
	pointer to @p ini_string), `memb_length` (the length of the member in bytes),

	`memb_num` (the index of the member in number of members), `format` (the format
	of the INI file), `foreach_other` (the custom argument @p user_data previously
	passed). If @p f_foreach returns a non-zero value the function
	`ini_array_split()` will be interrupted.

	Similarly to `strtok()` this function can be used only once for a given string.

	See example under `ini_array_collapse()`.

**/
int ini_array_split (
	char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach) (
		char *member,
		size_t memb_length,
		size_t memb_num,
		IniFormat format,
		void *foreach_other
	),
	void *user_data
) {

	/* Mask `abcd` (8 bits used): as in `ini_array_foreach()` */

	register uint8_t abcd	=	(delimiter ? 4 : 0) |
								(format.no_double_quotes << 1) |
								format.no_single_quotes;


	for (size_t offs = 0, counter = 0, idx = 0; !(abcd & 128); idx++) {

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
			offs = ltrim_h(ini_string, offs, _LIBCONFINI_WITH_EOL_);

			if (f_foreach(ini_string + offs, rtrim_h(ini_string + offs, idx - offs, _LIBCONFINI_WITH_EOL_), counter++, format, user_data)) {

				return CONFINI_FEINTR;

			}

			if (!(abcd & 132)) {

				idx = ltrim_h(ini_string, idx, _LIBCONFINI_WITH_EOL_) - 1;

			}

			offs = idx + 1;

		}

	}

	return _LIBCONFINI_SUCCESS_;

}


													/** @utility{ini_get_bool} **/
/**

	@brief			Checks whether a string matches *exactly* one of the booleans
					listed in the private constant `#INI_BOOLEANS` (case
					insensitive)
	@param			ini_string			A string to be checked
	@param			return_value		A value that is returned if no matching
										boolean has been found
	@return			The matching boolean value (0 or 1) or @p return_value if no
					boolean has been found

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be
	used as well).

	@include miscellanea/typed_ini.c

**/
signed int ini_get_bool (const char * const ini_string, const signed int return_value) {

	uint8_t bool_idx;
	size_t pair_idx, chr_idx;

	for (pair_idx = 0; pair_idx < sizeof(INI_BOOLEANS) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			for (chr_idx = 0; _LIBCONFINI_CHR_CASEFOLD_(ini_string[chr_idx]) == INI_BOOLEANS[pair_idx][bool_idx][chr_idx]; chr_idx++) {

				if (!ini_string[chr_idx]) {

					return bool_idx;

				}

			}

		}

	}

	return return_value;

}


												/** @utility{ini_get_lazy_bool} **/
/**

	@brief			Checks whether the first letter of a string matches the first
					letter of one of the booleans listed in the private constant
					`#INI_BOOLEANS` (case insensitive)
	@param			ini_string			A string to be checked
	@param			return_value		A value that is returned if no matching
										boolean has been found
	@return			The matching boolean value (0 or 1) or @p return_value if no
					boolean has been found

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be
	used as well).

**/
signed int ini_get_lazy_bool (const char * const ini_string, const signed int return_value) {

	uint8_t bool_idx;
	size_t pair_idx;

	for (pair_idx = 0; pair_idx < sizeof(INI_BOOLEANS) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			if (_LIBCONFINI_CHR_CASEFOLD_(*ini_string) == *INI_BOOLEANS[pair_idx][bool_idx]) {

				return bool_idx;

			}

		}

	}

	return return_value;

}



/* LINKS -- In case you don't want to `#include <stdlib.h>` in your source */

/**

	@alias{ini_get_int}		Link to [`atoi()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi)
	@alias{ini_get_lint}	Link to [`atol()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol)
	@alias{ini_get_llint}	Link to [`atoll()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll)
	@alias{ini_get_float}	Link to [`atof()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof)

**/

int (* const ini_get_int) (const char *ini_string) = &atoi;

long int (* const ini_get_lint) (const char *ini_string) = &atol;

long long int (* const ini_get_llint) (const char *ini_string) = &atoll;

double (* const ini_get_float) (const char *ini_string) = &atof;



/* VARIABLES */

_Bool INI_GLOBAL_LOWERCASE_MODE = _LIBCONFINI_FALSE_;
char *INI_GLOBAL_IMPLICIT_VALUE = (char *) 0;
size_t INI_GLOBAL_IMPLICIT_V_LEN = 0;



/** @endfnlist **/

/* EOF */

