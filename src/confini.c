/*	-*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-	*/

/**

	@file		confini.c
	@brief		libconfini functions
	@author		Stefano Gioffr&eacute;
	@copyright 	GNU Public License v3
	@date		2016-2017

**/

#include <stdlib.h>
#include <stdint.h>
#include "confini.h"


/**

	@struct		IniFormat

	Structure of the bitfield:

	- Bits 1-18: INI syntax
	- Bits 19-20: INI semantics
	- Bits 21-22: user's syntax (disabled entries)
	- Bits 23-24: unused

	@property	IniFormat::delimiter_symbol
					The symbol to be used as delimiter; if set to `0`, any space is delimiter
					(`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@property	IniFormat::semicolon
					The rule of the semicolon character (use enum `#IniComments` for this)
	@property	IniFormat::hash
					The rule of the hash character (use enum `#IniComments` for this)
	@property	IniFormat::multiline_entries
					Defines which class of entries are allowed to be multiline (use enum `#IniMultiline` for this)
	@property	IniFormat::case_sensitive
					If set to `1`, key and section names will not be rendered in lower case
	@property	IniFormat::no_spaces_in_names
					If set to `1`, key and section names containing spaces will be rendered as `#INI_UNKNOWN`.
					Note that setting `IniFormat::delimiter_symbol` to `INI_ANY_SPACE` will not automatically set
					this option to `1` (spaces will be still allowed in section names).					
	@property	IniFormat::no_single_quotes
					If set to `1`, the single-quote character (`'`) will be considered as a normal character
	@property	IniFormat::no_double_quotes
					If set to `1`, the double-quote character (`"`) will be considered as a normal character
	@property	IniFormat::implicit_is_not_empty
					If set to `1`, the dispatch of implicit keys (see @ref libconfini) will always
					assign to `IniDispatch::value` and to `IniDispatch::v_len` the global variables
					`#INI_IMPLICIT_VALUE` and `#INI_IMPLICIT_V_LEN` respectively; if set to `0`,
					implicit keys will be considered to be empty keys
	@property	IniFormat::do_not_collapse_values
					If set to `1`, sequences of more than one space in values (`/\s{2,}/`) will not be collapsed
	@property	IniFormat::no_disabled_after_space
					If set to `1`, prevents that what follows `/(?:^|\s+)[#;]\s/` be parsed as a disabled entry
	@property	IniFormat::disabled_can_be_implicit
					If set to `1`, comments non containing a delimiter symbol will not be parsed as
					disabled implicit keys, but as simple comments


	@struct		IniStatistics

	@property	IniStatistics::format
					The format of the INI file (see struct `IniFormat`)
	@property	IniStatistics::bytes
					The size of the parsed file in bytes
	@property	IniStatistics::members
					The size of the parsed file in members (nodes) -- this number equals the number of dispatches


	@struct		IniDispatch

	@property	IniDispatch::format
					The format of the INI file (see struct `IniFormat`)
	@property	IniDispatch::type
					The dispatch type (see enum `#IniNodeType`)
	@property	IniDispatch::data
					It can be a comment, a section path or a key name
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



		/*\
		|*|
		|*|   PRIVATE ENVIRONMENT
		|*|
		\*/



/* ALIASES */

#define _LIBCONFINI_BOOL_ unsigned char
#define _LIBCONFINI_FALSE_ 0
#define _LIBCONFINI_TRUE_ 1
#define _LIBCONFINI_BACKSLASH_ '\\'
#define _LIBCONFINI_OPEN_SECTION_ '['
#define _LIBCONFINI_CLOSE_SECTION_ ']'
#define _LIBCONFINI_SUBSECTION_ '.'
#define _LIBCONFINI_DOUBLE_QUOTES_ '"'
#define _LIBCONFINI_SINGLE_QUOTES_ '\''
#define _LIBCONFINI_SIMPLE_SPACE_ 32
#define _LIBCONFINI_SEMICOLON_ ';'
#define _LIBCONFINI_HASH_ '#'
#define _LIBCONFINI_LF_ '\n'
#define _LIBCONFINI_CR_ '\r'
/* In the future there will maybe be support for UTF-8 casefold, but for now only ASCII... */
#define _LIBCONFINI_CHR_CASEFOLD_(CHR) (CHR > 0x40 && CHR < 0x5b ? CHR | 0x60 : CHR)


/* FUNCTIONAL CONSTANTS */

/**

	@brief	 A list of possible string representations of boolean pairs

	There may be infinite pairs here, but be aware of the lazy behavior of ini_get_lazy_bool().
	Everything lowercase in this list!

**/
static const char * const _LIBCONFINI_BOOLEANS_[][2] = {
	{ "no", "yes" },
	{ "false", "true" },
	{ "0", "1" }
};

/** @brief	 UTF-8 BOM **/
static const char _LIBCONFINI_UTF8_BOM_[] = { 0xEF, 0xBB, 0xBF };

/*
	This may be any character, in theory... But after the left-trim of each line a leading
	space works pretty well as metacharacter.
*/
#define _LIBCONFINI_INLINE_MARKER_ '\t'



/* ABSTRACT UTILITIES */

/* The list of space characters -- do not change its order! */
static const char _LIBCONFINI_SPACES_[] = { _LIBCONFINI_SIMPLE_SPACE_, '\t', '\f', '\v', _LIBCONFINI_LF_, _LIBCONFINI_CR_ };

/* Possible lengths of array `_LIBCONFINI_SPACES_` */
#define _LIBCONFINI_JUST_S_T_ 2
#define _LIBCONFINI_NO_EOL_ 4
#define _LIBCONFINI_WITH_EOL_ 6

static inline _LIBCONFINI_BOOL_ is_some_space (const char chr, const uint8_t depth) {
	uint8_t idx;
	for (idx = 0; idx < depth && chr != _LIBCONFINI_SPACES_[idx]; idx++);
	return idx < depth;
}


/**

	@brief			Soft left trim -- does not change the buffer
	@param			lt_s			The target string
	@param			start_from		The offset where to start the left trim
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The offset of the first non-space character

**/
static inline size_t ltrim_s (const char * const lt_s, const size_t start_from, const uint8_t depth) {
	size_t lt_i = start_from;
	for (; lt_s[lt_i] && is_some_space(lt_s[lt_i], depth); lt_i++);
	return lt_i;
}


/**

	@brief			Hard left trim -- **does** change the buffer
	@param			lt_s			The target string
	@param			start_from		The offset where to start the left trim
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The offset of the first non-space character

**/
static inline size_t ltrim_h (char * const lt_s, const size_t start_from, const uint8_t depth) {
	size_t lt_i = start_from;
	for (; lt_s[lt_i] && is_some_space(lt_s[lt_i], depth); lt_s[lt_i++] = '\0');
	return lt_i;
}


/**

	@brief			Unescaped hard left trim (left trim of `/^(\s+|\\[\n\r])+/`) -- **does** change the buffer
	@param			ult_s			The target string
	@param			start_from		The offset where to start the left trim
	@return			The offset of the first non-space character

**/
static inline size_t ultrim_h (char * const ult_s, const size_t start_from) {

	uint8_t abacus;
	size_t ult_i = start_from;

	/*

	Mask `abacus` (5 bits used):

		FLAG_1		This is any space (`/[\t \v\f\n\r]/`)
		FLAG_2		This is not a backslash OR this is neither a backslash nor `/[\t \v\f]/` and previous was not a backslash
		FLAG_4		This is a backslash OR this is a new line and previous was a backslash
		FLAG_8		Continue the loop

	*/

	for (abacus = 10; abacus & 8; ult_i++) {

		abacus	=	(abacus & 2) && is_some_space(ult_s[ult_i], _LIBCONFINI_NO_EOL_) ? 11
					: ult_s[ult_i] == _LIBCONFINI_LF_ || ult_s[ult_i] == _LIBCONFINI_CR_ ? abacus | 3
					: (abacus & 2) && ult_s[ult_i] == _LIBCONFINI_BACKSLASH_ ? 12
					: abacus & 2;

		if (abacus & 1) {

			ult_s[ult_i] = '\0';

		}

		if (abacus == 15) {

			ult_s[ult_i - 1] = '\0';
			abacus &= 11;

		}

	}

	return abacus & 2 ? ult_i - 1 : ult_i - 2;

}


/**

	@brief			Soft right trim -- does not change the buffer
	@param			rt_s			The target string
	@param			length			The length of the string
	@param			depth			What is actually considered a space -- a subset of `_LIBCONFINI_SPACES_` ranging from 1 to 6
	@return			The length of the string until the last non-space character

**/
static inline size_t rtrim_s (const char * const rt_s, const size_t length, const uint8_t depth) {
	size_t rt_l = length;
	for (; rt_l > 0 && is_some_space(rt_s[rt_l - 1], depth); rt_l--);
	return rt_l;
}


/**

	@brief			Hard right trim -- **does** change the buffer
	@param			rt_s			The target string
	@param			length			The length of the string
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The length of the string until the last non-space character

**/
static inline size_t rtrim_h (char * const rt_s, const size_t length, const uint8_t depth) {
	size_t rt_l = length;
	for (; rt_l > 0 && is_some_space(rt_s[rt_l - 1], depth); rt_s[--rt_l] = '\0');
	return rt_l;
}


/**

	@brief			Unescaped soft right trim (right trim of `/(\s+|\\[\n\r])+$/`) -- does not change the buffer
	@param			urt_s			The target string
	@param			length			The length of the string
	@return			The length of the string until the last non-space character

**/
static inline size_t urtrim_s (const char * const urt_s, const size_t length) {

	size_t urt_l = length;

	/*

	Mask `abacus` (2 bits used):

		FLAG_1		Continue the loop
		FLAG_2		Next character is a new line character

	*/

	for (

		uint8_t abacus = 1;

			(
				abacus	=	urt_l < 1 ? 0
							: urt_s[urt_l - 1] == _LIBCONFINI_LF_ || urt_s[urt_l - 1] == _LIBCONFINI_CR_ ? 3
							: urt_s[urt_l - 1] == _LIBCONFINI_BACKSLASH_ ? abacus >> 1
							: is_some_space(urt_s[urt_l - 1], _LIBCONFINI_NO_EOL_) ? 1
							: 0
			) & 1;

		urt_l--

	);

	return urt_l;

}


/**

	@brief			Converts an ASCII string to lower case
	@param			string		The target string
	@return			Nothing

**/
static inline void string_tolower (char * const str) {
	for (char *ch_ptr = str; *ch_ptr; ++ch_ptr) {
		*ch_ptr = _LIBCONFINI_CHR_CASEFOLD_(*ch_ptr);
	}
}



/* CONCRETE UTILITIES */


/**

	@brief			Checks whether a character can represent the beginning of a disabled entry
					within a given format
	@param			chr			The character to check
	@param			format		The format of the INI file
	@return			1 if @p chr matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_parsable_char (const char chr, const IniFormat format) {
	return	(
				chr == _LIBCONFINI_SEMICOLON_ && format.semicolon == INI_PARSE_COMMENT
			) || (
				chr == _LIBCONFINI_HASH_ && format.hash == INI_PARSE_COMMENT
			);
}


/**

	@brief			Checks whether a character represents the opening of a comment within a given format
	@param			chr			The character to check
	@param			format		The format of the INI file
	@return			1 if @p chr matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_comment_char (const char chr, const IniFormat format) {
	return	(
				chr == _LIBCONFINI_SEMICOLON_ && format.semicolon != INI_NORMAL_CHARACTER
			) || (
				chr == _LIBCONFINI_HASH_ && format.hash != INI_NORMAL_CHARACTER
			);
}


/**

	@brief			Checks whether a character represents the beginning of a comment that must be ignored
					within a given format
	@param			chr			The character to check
	@param			format		The format of the INI file
	@return			1 if @p chr matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_forget_char (const char chr, const IniFormat format) {
	return (format.semicolon == INI_FORGET_COMMENT && chr == _LIBCONFINI_SEMICOLON_) || (format.hash == INI_FORGET_COMMENT && chr == _LIBCONFINI_HASH_);
}


/**

	@brief			Gets the position of the first delimiter out of quotes
	@param			string		The string to analyse
	@param			length		The length of the string
	@param			format		The format of the INI file
	@return			The offset of the delimiter or @p length if the delimiter has not
					been not found

**/
static inline size_t get_delimiter_pos (const char * const str, const size_t len, const IniFormat format) {

	size_t idx = 0;

	/*

	Mask `abacus` (5 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		We are in an odd sequence of backslashes

	*/

	for (

		uint8_t abacus = (format.no_double_quotes << 1) | format.no_single_quotes;

			idx < len && (
				(abacus & 12) || !(
					format.delimiter_symbol ?
						str[idx] == (char /* Neutralize the `unsigned` keyword used in the bitfield */) format.delimiter_symbol
						: is_some_space(str[idx], _LIBCONFINI_NO_EOL_)
				)
			);

		abacus	=	str[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 16
					: !(abacus & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 8
					: !(abacus & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 4
					: abacus & 15,
		idx++

	);

	return idx;

}


/**

	@brief			Replaces `/\\(\n\r?|\r\n?)s*[#;]/` or `/\\(\n\r?|\r\n?)/` with `"$1"`.
	@param			str				Target string
	@param			len				Length of the string
	@param			is_disabled		The string represents a disabled entry
	@param			format			The format of the INI file
	@return			The new length of the string

**/
static size_t unescape_cr_lf (char * const str, const size_t len, const _LIBCONFINI_BOOL_ is_disabled, const IniFormat format) {

	size_t idx, iter, lshift;
	_LIBCONFINI_BOOL_ is_escaped = _LIBCONFINI_FALSE_;
	uint8_t cr_or_lf = 5;

	for (is_escaped = _LIBCONFINI_FALSE_, idx = 0, lshift = 0; idx < len; idx++) {

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

	@brief			Out of quotes similar to ECMAScript `string.replace(/^[\n\r]\s*|(\s)+/g, " ")`
	@param			str			The string to collapse
	@param			format		The format of the INI file
	@return			The new length of the string

**/
static size_t collapse_spaces (char * const str, const IniFormat format) {

	/*

	Mask `abacus` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		More than one space is here

	*/

	uint8_t	abacus	=	(is_some_space(*str, _LIBCONFINI_WITH_EOL_) ? 32 : 0) |
									(format.no_double_quotes << 1) |
									format.no_single_quotes;

	size_t idx, lshift;

	for (lshift = 0, idx = 0; str[idx]; idx++) {

		if (!(abacus & 12) && is_some_space(str[idx], _LIBCONFINI_WITH_EOL_)) {

			if (abacus & 32) {

				lshift++;

			} else {

				str[idx - lshift] = _LIBCONFINI_SIMPLE_SPACE_;
				abacus |= 32;

			}

			abacus &= 47;

		} else {

			abacus	=	str[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 16
						: !(abacus & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 8
						: !(abacus & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 4
						: abacus & 47;

			abacus &= 31;

			if (lshift) {

					str[idx - lshift] = str[idx];

			}

		}

	}

	for (idx -= abacus & 32 ? ++lshift : lshift; str[idx]; str[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Sanitizes the name of a section
	@param			str			The target string
	@param			format		The format of the INI file
	@return			The new length of the string

	Out of quotes, similar to ECMAScript `section_name.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/\s+/g, " ")`

	A section name can start with a dot (append), but cannot end with a dot. Spaces sorrounding dots
	will be removed. Single quotes or double quotes (if active and present) prevent changes.

**/
static size_t sanitize_section_name (char * const str, const IniFormat format) {

	size_t idx, lshift;

	/*

	Mask `abacus` (11 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		The string does not start with a new line (const)
		FLAG_64		More than one space is here
		FLAG_128	At least one dot is here
		FLAG_256	One or more spaces follow a dot
		FLAG_512	More than one dot is here
		FLAG_1024	Continue the shift

	*/

	uint16_t abacus	=	(is_some_space(*str, _LIBCONFINI_WITH_EOL_) ? 256 : 32) |
				(format.no_double_quotes << 1) |
				format.no_single_quotes;

	for (lshift = 0, idx = 0; str[idx]; idx++) {

		if (!(abacus & 12) && is_some_space(str[idx], _LIBCONFINI_WITH_EOL_)) {

			if (abacus & 320) {

				lshift++;

			}

			if (!(abacus & 64)) {

				str[idx - lshift] = abacus & 128 ? _LIBCONFINI_SUBSECTION_ : _LIBCONFINI_SIMPLE_SPACE_;
				abacus &= 51;
				abacus |= 64;

			}

			abacus &= 2031;

		} else if (!(abacus & 12) && str[idx] == _LIBCONFINI_SUBSECTION_) {

			if ((abacus & 576) && (abacus & 32)) {

				lshift++;

			}

			abacus |= abacus & 576 ? 1440 : 1952;
			abacus &= 2031;

		} else {

				abacus	=	(
								(
									str[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 16
									: !(abacus & 22) && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 8
									: !(abacus & 25) && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 4
									: abacus & 2031
								) & 1087
							) | 1056;

		}

		if (abacus & 1024) {

			str[idx - lshift] = str[idx];
			abacus &= 1023;

		}

	}

	for (idx -= abacus & 960 ? ++lshift : lshift; str[idx]; str[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Removes all comment initializers (`#` or `;`) from the beginning of each line of a comment
	@param			commstr		The comment to sanitize
	@param			len			The length of commstr
	@param			format		The format of the INI file
	@return			The new length of the string

	- In multiline comments: `commstr.replace(/(^|\n\r?|\r\n?)[\t \v\f]*[#;]+/g, "$1")`
	- In single-line comments: `commstr.replace(/^[\t \v\f]*[#;]+/, "")`

**/
static size_t uncomment (char * const commstr, size_t len, const IniFormat format) {

	size_t lshift, idx;

	if (format.multiline_entries) {

		/* The comment is not multiline */

		for (lshift = 0; lshift < len && is_comment_char(commstr[lshift], format); lshift++);

		if (!lshift) {

			return len;

		}

		for (idx = lshift; idx < len; commstr[idx - lshift] = commstr[idx], idx++);

	} else {

		/* The comment is multiline */

		uint8_t abacus;

		/*

		Mask `abacus` (6 bits used):

			FLAG_1		Don't erase any character
			FLAG_2		We are in an odd sequence of backslashes
			FLAG_4		This character is a backslash
			FLAG_8		This character is a comment character and follows `/(\n\s*|\r\s*)/`
			FLAG_16		This character is a part of a group of spaces following a new line (`/(\n|\r)[\t \v\f]+/`)
			FLAG_32		This character is not a new line character (`/[^\r\n]/`)

		*/

		for (abacus = 11, lshift = 0, idx = 0; idx < len; idx++) {

			abacus	=	commstr[idx] == _LIBCONFINI_BACKSLASH_ ?
							((abacus & 35) | 32) ^ 2
						: commstr[idx] == _LIBCONFINI_LF_ || commstr[idx] == _LIBCONFINI_CR_ ?
							(abacus & 16) | ((abacus << 1) & 4)
						: !(abacus & 32) && is_comment_char(commstr[idx], format) ?
							(abacus & 40) | 8
						: !(abacus & 40) && is_some_space(commstr[idx], _LIBCONFINI_NO_EOL_) ?
							(abacus & 57) | 16
						:
							(abacus & 33) | 32;

			if (abacus & 28) {

				lshift++;

			}

			if (!(abacus & 25)) {

				commstr[idx - lshift] = commstr[idx];

			}

		}

	}

	for (idx -= lshift; idx < len; commstr[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Tries to determine the type of a member "as if it was active"
	@param			nodestr				String containing an individual node
	@param			len					Length of the node
	@param			allow_implicit		A boolean: `TRUE` if keys without a delimiter are allowed, `FALSE` otherwise
	@param			format				The format of the INI file
	@return			The node type (see header)

**/
static uint8_t get_type_as_active (
	const char * const nodestr,
	const size_t len,
	const _LIBCONFINI_BOOL_ allow_implicit,
	const IniFormat format
) {

	if (!len || *nodestr == (char /* Neutralize the `unsigned` keyword used in the bitfield */) format.delimiter_symbol || is_comment_char(*nodestr, format)) {

		return INI_UNKNOWN;

	}

	uint16_t abacus;
	size_t idx;

	if (*nodestr == _LIBCONFINI_OPEN_SECTION_) {

		if (format.no_spaces_in_names) {

			/*

				Search of the CLOSE_SECTION character and possible spaces in
				names -- i.e., ECMAScript `/[^\.\s]\s+[^\.\s]/g.test(nodestr)`.
				The algorithm is made more complex by the fact that LF and CR
				characters can still be escaped at this stage ("\\\n", "\\\r").

			*/

			/*

			Mask `abacus` (11 bits used):

				FLAG_1		We are in an odd sequence of backslashes
				FLAG_2		More than one backslash is here
				FLAG_4		Single quotes are not metacharacters (const)
				FLAG_8		Double quotes are not metacharacters (const)
				FLAG_16		Unescaped single quotes are odd until now
				FLAG_32		Unescaped double quotes are odd until now
				FLAG_64		We are not around a dot
				FLAG_128	This is a space
				FLAG_256	This is a new line
				FLAG_512	Section name end character found
				FLAG_1024	Name contains spaces

			*/

			for (abacus = (format.no_double_quotes << 3) | (format.no_single_quotes << 2), idx = 1; idx < len; idx++) {

				abacus	=	nodestr[idx] == _LIBCONFINI_BACKSLASH_ ?
								(abacus | (((abacus << 1) & 2) & (~abacus & 2))) ^ 1
							: !(abacus & 51) && nodestr[idx] == _LIBCONFINI_SUBSECTION_ ?
								abacus & 1968
							: !(abacus & 49) && is_some_space(nodestr[idx], _LIBCONFINI_NO_EOL_) ?
								(abacus & 2046) | (abacus & 2 ? 192 : 128)
							: !(abacus & 48) && (nodestr[idx] == _LIBCONFINI_LF_ || nodestr[idx] == _LIBCONFINI_CR_) ?
								(abacus & 2046) | (abacus & 2 ? 320 : 256)
							: !(abacus & 25) && nodestr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abacus ^ 32) & 1649) | ((abacus & 64) && (abacus & 384) ? 1088 : 64)
							: !(abacus & 37) && nodestr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abacus ^ 16) & 1649) | ((abacus & 64) && (abacus & 384) ? 1088 : 64)
							: !(abacus & 48) && nodestr[idx] == _LIBCONFINI_CLOSE_SECTION_ ?
								(abacus & 1968) | 512
							:
								(abacus & 1648) | ((abacus & 64) && (abacus & 384) ? 1088 : 64);

				if (abacus & 1024) {

					return INI_UNKNOWN;

				}

				if (abacus & 512) {

					return INI_SECTION;

				}

			}

		} else {

			/*

			Mask `abacus` (5 bits used):

				FLAG_1		Single quotes are not metacharacters (const)
				FLAG_2		Double quotes are not metacharacters (const)
				FLAG_4		Unescaped single quotes are odd until now
				FLAG_8		Unescaped double quotes are odd until now
				FLAG_16		We are in an odd sequence of backslashes

			*/

			for (abacus = (format.no_double_quotes << 1) | format.no_single_quotes, idx = 1; idx < len; idx++) {

				if (!(abacus & 12) && nodestr[idx] == _LIBCONFINI_CLOSE_SECTION_) {

					return INI_SECTION;

				}

				abacus	=	nodestr[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 16
							: !(abacus & 22) && nodestr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 8
							: !(abacus & 25) && nodestr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 4
							: abacus & 15;

			}

		}

		return INI_UNKNOWN;

	}

	/* It can be just a key... */

	/*

	Mask `abacus` (2 bits used):

		FLAG_1		The delimiter **must** be present
		FLAG_2		Search for spaces in names

	*/

	abacus	=	(format.no_spaces_in_names && format.delimiter_symbol ? 2 : 0) |
				(allow_implicit ? 0 : 1);

	if (abacus) {

		idx = get_delimiter_pos(nodestr, len, format);

		if ((abacus & 1) && idx == len) {

			return INI_UNKNOWN;

		}

		if (abacus & 2) {

			idx = urtrim_s(nodestr, idx) - 1;

			do {

				if (is_some_space(nodestr[idx], _LIBCONFINI_WITH_EOL_)) {

					return INI_UNKNOWN;

				}

			} while (idx-- > 0);

		}

	}

	return INI_KEY;

}


/**

	@brief			Examines a (single-/multi- line) segment and checks whether it contains sub-segments.
	@param			segment		Segment to examine
	@param			format		The format of the INI file
	@return			Number of segments found

**/
static size_t further_cuts (char * const segment, const IniFormat format) {

	_LIBCONFINI_BOOL_ forget_me;
	uint8_t abacus = (format.no_double_quotes << 1) | format.no_single_quotes;
	size_t idx, focus_at, unparsable_at, search_at = 0, segm_size = 0;

	search_for_cuts:

	if (!segment[search_at]) {

		goto no_more_cuts;

	}

	unparsable_at = 0;
	forget_me = is_forget_char(segment[search_at], format);

	if (!forget_me) {

		segm_size++;

	}

	/*

	Mask `abacus` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		The previous character was not a space
		FLAG_64		This can be a disabled entry
		FLAG_128	This is nothing but a (multiline) comment

	*/

	abacus		=	is_parsable_char(segment[search_at], format) && !(
						format.no_disabled_after_space && is_some_space(segment[search_at + 1], _LIBCONFINI_NO_EOL_)
					) ?
						(abacus & 3) | 64
					: !format.multiline_entries && (
						segment[search_at] == _LIBCONFINI_INLINE_MARKER_ || is_comment_char(segment[search_at], format)
					) ?
						(abacus & 3) | 128
					:
						abacus & 3;

	if (abacus & 192) {

		for (idx = ltrim_s(segment, search_at + 1, _LIBCONFINI_NO_EOL_); segment[idx]; idx++) {

			if (is_comment_char(segment[idx], format)) {

				/* Search for inline comments in (supposedly) disabled items */

				if (!(abacus & 172)) {

					focus_at = ltrim_s(segment, search_at + 1, _LIBCONFINI_NO_EOL_);

					if (get_type_as_active(segment + focus_at, idx - focus_at, format.disabled_can_be_implicit, format)) {

						if (!is_forget_char(segment[idx], format)) {

							segment[idx] = _LIBCONFINI_INLINE_MARKER_;
							segm_size++;

						}

						segment[idx - 1] = '\0';

						if (format.multiline_entries) {

							unparsable_at = idx + 1;
							break;

						}

					} else if (format.multiline_entries) {

						unparsable_at = search_at + 1;
						break;

					}

				}

				abacus &= 239;
				abacus |= 32;

			} else if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

				/* Search for `/\\(?:\n\r?|\r\n?)\s*[^;#]/` in multiline disabled items */

				focus_at = ltrim_s(segment, search_at + 1, _LIBCONFINI_NO_EOL_);
				idx = ltrim_s(segment, idx + 1, _LIBCONFINI_WITH_EOL_);

				if (
					forget_me ?
						!is_forget_char(segment[idx], format)
					:
						is_forget_char(segment[idx], format) || !(
							(
								format.multiline_entries < 2 && is_parsable_char(segment[idx], format) && !(
									format.no_disabled_after_space && (abacus & 64) && is_some_space(segment[idx + 1], _LIBCONFINI_NO_EOL_)
								)
							) || (
								!format.multiline_entries && is_comment_char(segment[idx], format) && !(
									(abacus & 64) && get_type_as_active(
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
					search_at = idx;
					goto search_for_cuts;

				}

				abacus &= 207;

			} else if (is_some_space(segment[idx], _LIBCONFINI_NO_EOL_)) {

				abacus &= 207;

			} else {

				abacus	=	segment[idx] == _LIBCONFINI_BACKSLASH_ ? (abacus | 32) ^ 16
							: !(abacus & 22) && segment[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? (abacus | 32) ^ 8
							: !(abacus & 25) && segment[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? (abacus | 32) ^ 4
							: (abacus & 239) | 32;

			}

		}

		if (format.multiline_entries && !unparsable_at) {

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

		Mask `abacus` (7 bits used):

			FLAG_1		Single quotes are not metacharacters (const)
			FLAG_2		Double quotes are not metacharacters (const)
			FLAG_4		Unescaped single quotes are odd until now
			FLAG_8		Unescaped double quotes are odd until now
			FLAG_16		We are in an odd sequence of backslashes
			FLAG_32		This is not a hash nor a semicolon character
			FLAG_64		The previous character is not a space

		*/

		for (abacus = (abacus & 3) | 96, idx = search_at + 1; segment[idx]; idx++) {

			abacus	=	is_comment_char(segment[idx], format) ?
							abacus & 79
						: is_some_space(segment[idx], _LIBCONFINI_WITH_EOL_) ?
							(abacus & 47) | 32
						: segment[idx] == _LIBCONFINI_BACKSLASH_ ?
							(abacus | 96) ^ 16
						: !(abacus & 22) && segment[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
							(abacus | 96) ^ 8
						: !(abacus & 25) && segment[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
							(abacus | 96) ^ 4
						:
							(abacus & 111) | 96;

			if (!(abacus & 124)) {

				segment[idx - 1] = '\0';

				if (!is_forget_char(segment[idx], format)) {

					segment[idx] = _LIBCONFINI_INLINE_MARKER_;

					if (format.multiline_entries) {

						segm_size++;

					}

				}

				if (format.multiline_entries) {

					unparsable_at = idx + 1;
					break;

				}

				search_at = idx;
				goto search_for_cuts;

			}

		}

	}

	if (unparsable_at) {

		/* Cut unparsable multiline comments */

		for (idx = unparsable_at; segment[idx]; idx++) {

			if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

				search_at = ultrim_h(segment, idx);
				goto search_for_cuts;

			}

		}

	}

	no_more_cuts:
	return segm_size;

}


/*----------------------------------------------------*/



		/*\
		|*|
		|*|   GLOBAL ENVIRONMENT
		|*|
		\*/



/* LIBRARY'S MAIN FUNCTIONS */

/**

	@brief			Parses an INI file and dispatches its content
	@param			ini_file		The `FILE` structure pointing to the INI file to parse
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the dispatch, or NULL
	@param			f_foreach		The function that will be invoked for each dispatch, or NULL
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

	The function @p f_init will be invoked with two arguments: `statistics` (a pointer to an `IniStatistics`
	object containing some properties about the file read) and `init_other` (the custom argument @p user_data
	previously passed). If @p f_init returns a non-zero value the caller function will be interrupted.

	The function @p f_foreach will be invoked with two arguments: `dispatch` (a pointer to an `IniDispatch`
	object containing the parsed member of the INI file) and `foreach_other` (the custom argument @p user_data
	previously passed). If @p f_foreach returns a non-zero value the caller function will be interrupted.

**/
int load_ini_file (
	FILE * const ini_file,
	const IniFormat format,
	const int (* const f_init) (
		IniStatistics *statistics,
		void *init_other
	),
	const int (* const f_foreach) (
		IniDispatch *dispatch,
		void *foreach_other
	),
	void *user_data
) {

	fseek(ini_file, 0, SEEK_END);

	size_t tmp_size_1 = ftell(ini_file);

	#define __N_BYTES__ tmp_size_1

	char * const cache = (char *) malloc((__N_BYTES__ + 1) * sizeof(char));

	if (cache == NULL) {

		return CONFINI_ENOMEM;

	}

	int return_value = 0;

	rewind(ini_file);

	if (fread(cache, sizeof(char), __N_BYTES__, ini_file) < __N_BYTES__) {

		return_value = CONFINI_EIO;
		goto free_and_exit;

	}

	cache[__N_BYTES__] = '\0';

	_LIBCONFINI_BOOL_ tmp_bool;
	uint16_t abacus;
	size_t idx, node_at, tmp_size_2, tmp_size_3;

	/* PART ONE: Examine and isolate each segment */

	#define __IS_ESCAPED__ tmp_bool
	#define __EOL_ID__ abacus
	#define __N_MEMBERS__ tmp_size_2
	#define __SHIFT_LEN__ tmp_size_3

	__SHIFT_LEN__ = /* UTF-8 BOM */ *cache == *_LIBCONFINI_UTF8_BOM_ && cache[1] == _LIBCONFINI_UTF8_BOM_[1] && cache[2] == _LIBCONFINI_UTF8_BOM_[2] ? 3 : 0;

	/* Alternatively... */

	/*

	__SHIFT_LEN__ = (uint32_t) (((unsigned char) *cache) << 16 | ((unsigned char) cache[1]) << 8 | ((unsigned char) cache[2])) == 0xEFBBBF ? 3 : 0;

	*/

	for (

		__N_MEMBERS__ = 0,
		__EOL_ID__ = 5,
		__IS_ESCAPED__ = _LIBCONFINI_FALSE_,
		node_at = 0,
		idx = __SHIFT_LEN__;

			idx < __N_BYTES__;

		idx++

	) {

		cache[idx - __SHIFT_LEN__] = cache[idx];

		if (cache[idx] == _LIBCONFINI_SPACES_[__EOL_ID__] || cache[idx] == _LIBCONFINI_SPACES_[__EOL_ID__ ^= 1]) {

			if (format.multiline_entries < 3 && __IS_ESCAPED__) {

				idx += cache[idx + 1] == _LIBCONFINI_SPACES_[__EOL_ID__ ^ 1];

			} else {

				cache[idx] = '\0';
				__N_MEMBERS__ += further_cuts(cache + ultrim_h(cache, node_at), format);
				node_at = idx + 1;

			}

			__IS_ESCAPED__ = _LIBCONFINI_FALSE_;

		} else if (cache[idx] == _LIBCONFINI_BACKSLASH_) {

			__IS_ESCAPED__ = !__IS_ESCAPED__;

		} else if (cache[idx]) {

			__IS_ESCAPED__ = _LIBCONFINI_FALSE_;

		} else {

			/* Remove `NUL` characters in the buffer (if any) */

			__SHIFT_LEN__++;

		}

	}

	const size_t len = idx - __SHIFT_LEN__;

	while (idx > len) {

		cache[--idx] = '\0';

	}

	__N_MEMBERS__ += further_cuts(cache + ultrim_h(cache, node_at), format);

	IniStatistics this_doc = {
		.format = format,
		.bytes = __N_BYTES__,
		.members = __N_MEMBERS__
	};

	#undef __SHIFT_LEN__
	#undef __N_MEMBERS__
	#undef __EOL_ID__
	#undef __IS_ESCAPED__
	#undef __N_BYTES__

	/* Debug */

	/*

	for (size_t tmp = 0; tmp < len + 1; tmp++) {
		putchar(cache[tmp] == 0 ? '$' : cache[tmp]);
	}
	putchar(_LIBCONFINI_LF_);

	*/

	if (f_init && f_init(&this_doc, user_data)) {

		return_value = CONFINI_EIINTR;
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

	IniDispatch this_d = {
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

		if (this_d.dispatch_id >= this_doc.members) {

			return_value = CONFINI_EFEOOR;
			goto free_and_exit;

		}

		this_d.data = cache + node_at;
		this_d.value = cache + idx;
		this_d.type = 0;
		this_d.d_len = idx - node_at;
		this_d.v_len = 0;

		if (is_parsable_char(*this_d.data, format)) {

			parse_at = ltrim_s(cache, node_at + 1, _LIBCONFINI_NO_EOL_);

			/*

				Search for inline comments left unmarked _inside_ a parsable comment:
				if found it means that the comment is not parsable.

			*/

			/*

			Mask `abacus` (10 bits used):

				FLAG_1		Single quotes are not metacharacters (const)
				FLAG_2		Double quotes are not metacharacters (const)
				FLAG_4		Allow disabled after space (const)
				FLAG_8		Unescaped single quotes are odd until now
				FLAG_16		Unescaped double quotes are odd until now
				FLAG_32		We are in an odd sequence of backslashes
				FLAG_64		We are in `\n[\t \v\f]*`
				FLAG_128	Last was not `[\t \v\f]` OR `FLAG_64 == FALSE`
				FLAG_256	Last was not `\s[;#]`, or was it but was semantic rather than syntactic
				FLAG_512	Continue the loop

			*/

			abacus	=	512 |
						(format.no_disabled_after_space ? 0 : 4) |
						(format.no_double_quotes << 1) |
						format.no_single_quotes;

			for (__ITER__ = 1; (abacus & 512) && __ITER__ < this_d.d_len; __ITER__++) {

				abacus	=	this_d.data[__ITER__] == _LIBCONFINI_LF_ || this_d.data[__ITER__] == _LIBCONFINI_CR_ ?
								abacus | 448
							: is_comment_char(this_d.data[__ITER__], format) ?
								(
									(abacus & 64) && !is_parsable_char(this_d.data[__ITER__], format) ?
										abacus & 31
									: abacus & 128 ?
										abacus & 543
									: abacus & 24 ?
										(abacus & 799) | 256
									:
										abacus & 287
								)
							: is_some_space(this_d.data[__ITER__], _LIBCONFINI_NO_EOL_) ?
								(
									abacus & 64 ?
										(abacus & 991) | 448
									: abacus & 260 ?
										(abacus & 799) | 256
									:
										abacus & 31
								)
							: this_d.data[__ITER__] == _LIBCONFINI_BACKSLASH_ ?
								((abacus & 831) | 256) ^ 32
							: !(abacus & 42) && this_d.data[__ITER__] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								((abacus & 799) | 256) ^ 16
							: !(abacus & 49) && this_d.data[__ITER__] == _LIBCONFINI_SINGLE_QUOTES_ ?
								((abacus & 799) | 256) ^ 8
							:
								(abacus & 799) | 256;

			}

			this_d.type		=	__ITER__ == this_d.d_len ?
									get_type_as_active(cache + parse_at, idx - parse_at, format.disabled_can_be_implicit, format)
								:
									0;

			if (this_d.type) {

				this_d.data = cache + parse_at;
				this_d.d_len = idx - parse_at;

			}

			this_d.type |= 4;

		} else if (is_comment_char(*this_d.data, format)) {

			this_d.type = INI_COMMENT;

		} else if (*this_d.data == _LIBCONFINI_INLINE_MARKER_) {

			this_d.type = INI_INLINE_COMMENT;

		} else {

			this_d.type = get_type_as_active(this_d.data, this_d.d_len, _LIBCONFINI_TRUE_, format);

		}

		if (__CURR_PARENT_LEN__ && *subparent_str) {

			__ITER__ = 0;

			do {

				curr_parent_str[__ITER__ + __CURR_PARENT_LEN__] = subparent_str[__ITER__];

			} while (subparent_str[__ITER__++]);

			__CURR_PARENT_LEN__ += __ITER__ - 1;
			subparent_str = curr_parent_str + __CURR_PARENT_LEN__;

		}

		if (__PARENT_IS_DISABLED__ && !(this_d.type & 4)) {

			real_parent_str[__REAL_PARENT_LEN__] = '\0';
			__CURR_PARENT_LEN__ = __REAL_PARENT_LEN__;
			curr_parent_str = real_parent_str;
			__PARENT_IS_DISABLED__ = _LIBCONFINI_FALSE_;

		} else if (!__PARENT_IS_DISABLED__ && this_d.type == INI_DISABLED_SECTION) {

			__REAL_PARENT_LEN__ = __CURR_PARENT_LEN__;
			real_parent_str = curr_parent_str;
			__PARENT_IS_DISABLED__ = _LIBCONFINI_TRUE_;

		}

		this_d.append_to = curr_parent_str;
		this_d.at_len = __CURR_PARENT_LEN__;

		this_d.d_len		=	this_d.type == INI_COMMENT ? 
									uncomment(this_d.data, idx - node_at, format)
								: this_d.type == INI_INLINE_COMMENT ? 
									uncomment(++this_d.data, idx - node_at - 1, format)
								: format.multiline_entries < 3 ?
									unescape_cr_lf(this_d.data, idx - node_at, this_d.type & 4, format)
								:
									idx - node_at;

		switch (this_d.type) {

			/*

			case INI_UNKNOWN:

				break;

			*/

			/* Do nothing */

			case INI_SECTION:
			case INI_DISABLED_SECTION:

				*this_d.data++ = '\0';

				/*

				Mask `abacus` (5 bits used):

					FLAG_1		Single quotes are not metacharacters (const)
					FLAG_2		Double quotes are not metacharacters (const)
					FLAG_4		Unescaped single quotes are odd until now
					FLAG_8		Unescaped double quotes are odd until now
					FLAG_16		We are in an odd sequence of backslashes

				*/

				for (

					abacus = (format.no_double_quotes << 1) | format.no_single_quotes, __ITER__ = 0;

						this_d.data[__ITER__] && (
							(abacus & 12) || this_d.data[__ITER__] != _LIBCONFINI_CLOSE_SECTION_
						);

					abacus	=	this_d.data[__ITER__] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 16
								: !(abacus & 22) && this_d.data[__ITER__] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 8
								: !(abacus & 25) && this_d.data[__ITER__] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 4
								: abacus & 15,

					__ITER__++

				);

				while (this_d.data[__ITER__]) {

					this_d.data[__ITER__++] = '\0';

				}

				this_d.d_len = sanitize_section_name(this_d.data, format);

				if (__CURR_PARENT_LEN__ && *this_d.data == _LIBCONFINI_SUBSECTION_) {

					subparent_str = this_d.data;

				} else {

					if (!__CURR_PARENT_LEN__ && *this_d.data == _LIBCONFINI_SUBSECTION_) {

						curr_parent_str = this_d.data + 1;
						__CURR_PARENT_LEN__ = this_d.d_len - 1;

					} else {

						curr_parent_str = this_d.data;
						__CURR_PARENT_LEN__ = this_d.d_len;

					}

					subparent_str = cache + idx;
					this_d.append_to = subparent_str;
					this_d.at_len = 0;

				}

				if (INI_INSENSITIVE_LOWERCASE && !format.case_sensitive) {

					string_tolower(this_d.data);

				}

				break;

			case INI_KEY:
			case INI_DISABLED_KEY:

				__ITER__ = get_delimiter_pos(this_d.data, this_d.d_len, format);

				if (this_d.d_len && __ITER__ && __ITER__ < this_d.d_len) {

					this_d.data[__ITER__] = '\0';

					if (format.do_not_collapse_values) {

						this_d.value = this_d.data + ltrim_h(this_d.data, __ITER__ + 1, _LIBCONFINI_WITH_EOL_);
						this_d.v_len = rtrim_h(this_d.value, this_d.d_len + this_d.data - this_d.value, _LIBCONFINI_WITH_EOL_);

					} else {

						this_d.value = this_d.data + __ITER__ + 1;
						this_d.v_len = collapse_spaces(this_d.value, format);

					}

				} else if (format.implicit_is_not_empty) {

					this_d.value = INI_IMPLICIT_VALUE;
					this_d.v_len = INI_IMPLICIT_V_LEN;
				}

				this_d.d_len = collapse_spaces(this_d.data, format);

				if (INI_INSENSITIVE_LOWERCASE && !format.case_sensitive) {

					string_tolower(this_d.data);

				}

				break;

			case INI_COMMENT:
			case INI_INLINE_COMMENT:

				this_d.append_to = cache + idx;
				this_d.at_len = 0;

		}

		if (f_foreach(&this_d, user_data)) {

			return_value = CONFINI_EFEINTR;
			goto free_and_exit;

		}

		this_d.dispatch_id++;
		node_at = idx + 1;

	}

	free_and_exit:

	#undef __ITER__
	#undef __CURR_PARENT_LEN__
	#undef __REAL_PARENT_LEN__
	#undef __PARENT_IS_DISABLED__

	free(cache);
	return return_value;


}


/**

	@brief			Parses an INI file and dispatches its content
	@param			path			The path of the INI file
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the dispatch, or NULL
	@param			f_foreach		The function that will be invoked for each dispatch, or NULL
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

	For the two parameters @p f_init and @p f_foreach see function `load_ini_file()`.

**/
int load_ini_path (
	const char * const path,
	const IniFormat format,
	const int (* const f_init) (
		IniStatistics *statistics,
		void *init_other
	),
	const int (* const f_foreach) (
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



/* FUNCTIONS FOR THE USER (NOT USED BY LIBCONFINI) */


/**

	@brief			Sets the value of the global variable `#INI_INSENSITIVE_LOWERCASE`
	@param			b_lowercase			The new value 
	@return			Nothing

	If @p b_lowercase is any non-zero value key and section names in case-insensitive INI formats will be dispatched lowercase, verbatim otherwise (default value: non-zero).

**/
void ini_dispatch_case_insensitive_lowercase (int b_lowercase) {

	INI_INSENSITIVE_LOWERCASE = b_lowercase;

}


/**

	@brief			Sets the valued used for implicit keys
	@param			implicit_value		The string to be used as implicit value (usually `"YES"`, or `"TRUE"`)
	@param			implicit_v_len		The length of @p implicit_value (usually `0`, independently of its real length)
	@return			Nothing

**/
void ini_set_implicit_value (char * const implicit_value, const size_t implicit_v_len) {

	INI_IMPLICIT_VALUE = implicit_value;
	INI_IMPLICIT_V_LEN = implicit_v_len;

}


/**

	@brief			Converts an IniFormat into an ::IniFormatId
	@param			source		The IniFormat to be read
	@return			The mask representing the format

**/
IniFormatId ini_format_get_id (const IniFormat source) {

	unsigned short int bitpos = 0;
	IniFormatId mask = 0;

	#define __CALC_FORMAT_ID__(SIZE, PROPERTY, IGNORE_ME) \
		mask |= source.PROPERTY << bitpos;\
		bitpos += SIZE;

	_LIBCONFINI_EXPAND_MODEL_FORMAT_AS_(__CALC_FORMAT_ID__)

	#undef __CALC_FORMAT_ID__

	return mask;

}


/**

	@brief			Sets all the values of an IniFormat by reading them from an ::IniFormatId
	@param			dest_format		The IniFormat to be set
	@param			mask			The `#IniFormatId` to be read
	@return			Nothing

**/
void ini_format_set_to_id (IniFormat *dest_format, IniFormatId format_id) {

	#define __MAX_1_BITS__ 1
	#define __MAX_2_BITS__ 3
	#define __MAX_3_BITS__ 7
	#define __MAX_4_BITS__ 15
	#define __MAX_5_BITS__ 31
	#define __MAX_6_BITS__ 63
	#define __MAX_7_BITS__ 127
	#define __MAX_8_BITS__ 255
	#define __READ_FORMAT_ID__(SIZE, PROPERTY, IGNORE_ME) \
		dest_format->PROPERTY = format_id & __MAX_##SIZE##_BITS__;\
		format_id >>= SIZE;

	_LIBCONFINI_EXPAND_MODEL_FORMAT_AS_(__READ_FORMAT_ID__)

	#undef __READ_FORMAT_ID__
	#undef __MAX_8_BITS__
	#undef __MAX_7_BITS__
	#undef __MAX_6_BITS__
	#undef __MAX_5_BITS__
	#undef __MAX_4_BITS__
	#undef __MAX_3_BITS__
	#undef __MAX_2_BITS__
	#undef __MAX_1_BITS__

}


/**

	@brief			Compares two simple strings and checks if they match
	@param			simple_string_a		The first simple string
	@param			simple_string_b		The second simple string
	@return			A boolean: `TRUE` if the two strings match, `FALSE` otherwise

	Simple strings are user-given strings or the result of `ini_unquote()`. The following properties are read
	from argument @p format:

	- `format.case_sensitive`

**/
short int ini_string_match_ss (const char * const simple_string_a, const char * const simple_string_b, const IniFormat format) {

	short int are_equal = _LIBCONFINI_TRUE_;
	size_t idx = 0;

	if (format.case_sensitive) {

		do {

			are_equal = simple_string_a[idx] == simple_string_b[idx];

		} while (are_equal && simple_string_a[idx++]);

	} else {

		do {

			are_equal = _LIBCONFINI_CHR_CASEFOLD_(simple_string_a[idx]) == _LIBCONFINI_CHR_CASEFOLD_(simple_string_b[idx]);

		} while (are_equal && simple_string_a[idx++]);

	}


	return are_equal;
}


/**

	@brief			Compares an INI string with a simple string and checks if they match according to a format
	@param			ini_string			The INI string escaped according to @p format
	@param			simple_string		The simple string
	@param			format			The format of the INI file
	@return			A boolean: `TRUE` if the two strings match, `FALSE` otherwise

	INI strings are the strings typically dispatched by `load_ini_file()` and `load_ini_path()`, which may contain quotes and
	the three escaping sequences `\\`, `\'` and `\"`. Simple strings are user-given strings or the result of `ini_unquote()`.

	The following properties are read from argument @p format:

	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_entries`
	- `format.case_sensitive`

**/
short int ini_string_match_si (const char * const simple_string, const char * const ini_string, const IniFormat format) {

	if (format.no_double_quotes && format.no_single_quotes && format.multiline_entries == INI_NO_MULTILINE) {

		/* There are no escape sequences. I will perform a simple comparison between strings. */

		return ini_string_match_ss(simple_string, ini_string, format);

	}

	uint8_t abacus;
	short int are_equal = _LIBCONFINI_TRUE_;
	size_t idx_i = 0, idx_s = 0, nbacksl = 0;

	abacus = (format.no_double_quotes << 1) | format.no_single_quotes;

	/*

	Mask `abacus` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		This is an unescaped single quote and format supports single quotes
		FLAG_32		This is an unescaped double quote and format supports double quotes

	*/

	do {

		abacus &= 3;

		check_metachr:

		while (ini_string[idx_i] == _LIBCONFINI_BACKSLASH_) {

			nbacksl++;
			idx_i++;

		}

		abacus	=	!(abacus & 6) && ini_string[idx_i] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						(abacus & 15) | 32
					: !(abacus & 9) && ini_string[idx_i] == _LIBCONFINI_SINGLE_QUOTES_ ?
						(abacus & 15) | 16
					:
						abacus & 15;

		if (((abacus ^ 32) & 36) && ((abacus ^ 16) & 24)) {

			nbacksl++;

		} else if (!(nbacksl & 1)) {

			abacus ^= (abacus >> 2) & 12;
			idx_i++;
			goto check_metachr;

		}

		nbacksl = (nbacksl >> 1) + 1;

		while (--nbacksl && are_equal) {

			are_equal = simple_string[idx_s] && simple_string[idx_s++] == _LIBCONFINI_BACKSLASH_;

		}

		if (
			are_equal && (
				format.case_sensitive ?
					ini_string[idx_i] == simple_string[idx_s]
				:
					_LIBCONFINI_CHR_CASEFOLD_(ini_string[idx_i]) == _LIBCONFINI_CHR_CASEFOLD_(simple_string[idx_s])
			)
		) {

			nbacksl = 0;
			idx_s++;

		} else {

			are_equal = _LIBCONFINI_FALSE_;

		}

	} while (are_equal && ini_string[idx_i++]);

	return are_equal;

}


/**

	@brief			Compares two INI strings and checks if they match according to a format
	@param			ini_string_a			The first INI string unescaped according to @p format
	@param			ini_string_b			The second INI string unescaped according to @p format
	@param			format			The format of the INI file
	@return			A boolean: `TRUE` if the two strings match, `FALSE` otherwise

	INI strings are the strings typically dispatched by `load_ini_file()` and `load_ini_path()`, which may contain quotes and
	the three escaping sequences `\\`, `\'` and `\"`.

	The following properties are read from argument @p format:

	- `format.no_double_quotes`
	- `format.no_single_quotes`
	- `format.multiline_entries`
	- `format.case_sensitive`

**/
short int ini_string_match_ii (const char * const ini_string_a, const char * const ini_string_b, const IniFormat format) {

	if (format.no_double_quotes && format.no_single_quotes && format.multiline_entries == INI_NO_MULTILINE) {

		/* There are no escape sequences. I will perform a simple comparison between strings. */

		return ini_string_match_ss(ini_string_a, ini_string_b, format);

	}

	uint8_t side = 0, a_abacus[2];
	const char * const a_str[2] = { ini_string_a, ini_string_b };
	short int are_equal = _LIBCONFINI_TRUE_;
	size_t a_idx[2] = { 0, 0 }, a_nbacksl[2] = { 0, 0 };

	a_abacus[1] = *a_abacus = (format.no_double_quotes << 1) | format.no_single_quotes;

	/*

	Mask `a_abacus` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		This is an unescaped single quote and format supports single quotes
		FLAG_32		This is an unescaped double quote and format supports double quotes

	*/

	do {

		change_side:

		side ^= 1;
		a_abacus[side] &= 3;

		check_metachr:

		while (a_str[side][a_idx[side]] == _LIBCONFINI_BACKSLASH_) {

			a_nbacksl[side]++;
			a_idx[side]++;

		}

		a_abacus[side]	=	!(a_abacus[side] & 6) && a_str[side][a_idx[side]] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								(a_abacus[side] & 15) | 32
							: !(a_abacus[side] & 9) && a_str[side][a_idx[side]] == _LIBCONFINI_SINGLE_QUOTES_ ?
								(a_abacus[side] & 15) | 16
							:
								a_abacus[side] & 15;

		if (!(a_nbacksl[side] & 1) && !(((a_abacus[side] ^ 32) & 36) && ((a_abacus[side] ^ 16) & 24))) {

			a_abacus[side] ^= (a_abacus[side] >> 2) & 12;
			a_idx[side]++;
			goto check_metachr;

		}

		if (side) {

			goto change_side;

		}

		if (
			a_nbacksl[1] >> 1 == *a_nbacksl >> 1 && (
				format.case_sensitive ?
					ini_string_a[*a_idx] == ini_string_b[a_idx[1]]
				:
					_LIBCONFINI_CHR_CASEFOLD_(ini_string_a[*a_idx]) == _LIBCONFINI_CHR_CASEFOLD_(ini_string_b[a_idx[1]])
			)
		) {

			a_nbacksl[1] = *a_nbacksl = 0;
			a_idx[1]++;

		} else {

			are_equal = _LIBCONFINI_FALSE_;

		}

	} while (are_equal && ini_string_a[(*a_idx)++]);

	return are_equal;

}


/**

	@brief			Unescapes `\\`, `\'` and `\"` and removes all unescaped quotes (if single/double quotes are active)
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be used as well). If the string does
	not contain quotes, or if quotes are considered to be normal characters, no changes will be made.

**/
size_t ini_unquote (char * const ini_string, const IniFormat format) {

	size_t idx;

	if (format.no_double_quotes && format.no_single_quotes && format.multiline_entries == INI_NO_MULTILINE) {

		/* There are no escape sequences. I will just return the length of the string. */

		for (idx = 0; ini_string[idx]; idx++);

		return idx;

	}

	uint8_t abacus = (format.no_double_quotes << 1) | format.no_single_quotes;
	size_t lshift, nbacksl;

	/*

	Mask `abacus` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		This is an unescaped single quote and format supports single quotes
		FLAG_32		This is an unescaped double quote and format supports double quotes

	*/

	for (lshift = 0, nbacksl = 0, idx = 0; ini_string[idx]; idx++) {

		abacus		=	!(abacus & 6) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
							(abacus & 15) | 32
						: !(abacus & 9) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
							(abacus & 15) | 16
						:
							abacus & 15;

		if (!(nbacksl & 1) && !(((abacus ^ 32) & 36) && ((abacus ^ 16) & 24))) {

			abacus ^= (abacus >> 2) & 12; 
			lshift++;
			continue;

		}

		if (ini_string[idx] == _LIBCONFINI_BACKSLASH_) {

			nbacksl++;

		} else {

			if ((nbacksl & 1) && (abacus & 48)) {

				lshift++;

			}

			lshift += nbacksl >> 1;
			nbacksl = 0;

		}

		if (lshift) {

			ini_string[idx - lshift] = ini_string[idx];

		}

	}

	for (idx -= lshift + (nbacksl >> 1); ini_string[idx]; ini_string[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Gets the length of an INI array
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members -- if zero (`INI_ANY_SPACE`)
									any space is delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The length of the INI array

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be used as well).

**/
size_t ini_array_get_length (const char * const ini_string, const char delimiter, const IniFormat format) {

	size_t arr_length = 0;
	size_t idx;
	uint8_t abacus;

	/*

	Mask `abacus` (8 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Delimiter is not any space (const)
		FLAG_8		Unescaped single quotes are odd until now
		FLAG_16		Unescaped double quotes are odd until now
		FLAG_32		We are in an odd sequence of backslashes
		FLAG_64		This is not a delimiter
		FLAG_128	Stop the loop

	*/

	abacus	=	(delimiter ? 4 : 0) |
				(format.no_double_quotes << 1) |
				format.no_single_quotes;

	for (idx = 0; !(abacus & 128); idx++) {

		abacus	=	(delimiter ? ini_string[idx] == delimiter : is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_)) ?
						abacus & 159
					: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abacus | 64) ^ 32
					: !(abacus & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						((abacus & 223) | 64) ^ 16
					: !(abacus & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						((abacus & 223) | 64) ^ 8
					: ini_string[idx] ?
						(abacus & 223) | 64
					: 128;

		if (!(abacus & 88)) {

			if (!(abacus & 132)) {

				idx = ltrim_s(ini_string, idx, _LIBCONFINI_WITH_EOL_) - 1;

			}

			arr_length++;

		}

	}

	return arr_length;

}


/**

	@brief			Calls a custom function for each member of an INI array -- useful for read-only (const)
					stringified arrays
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members -- if zero (`INI_ANY_SPACE`) any
									space is delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@param			f_foreach		The function that will be invoked for each array member
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be used as well).

	The function @p f_foreach will be invoked with six arguments: `fullstring` (a pointer to @p ini_string),
	`memb_offset` (the offset of the member in bytes), `memb_length` (the length of the member in bytes),
	`index` (the index of the member in number of members), `format` (the format of the INI file),
	`foreach_other` (the custom argument @p user_data previously passed). If @p f_foreach returns a non-zero
	value the function `ini_array_foreach()` will be interrupted.

**/
int ini_array_foreach (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach) (
		const char *fullstring,
		size_t memb_offset,
		size_t memb_length,
		size_t index,
		IniFormat format,
		void *foreach_other
	),
	void *user_data
) {

	uint8_t abacus;
	size_t idx, offs, counter;

	/* Mask `abacus` (8 bits used): as in `ini_array_get_length()` */

	abacus	=	(delimiter ? 4 : 0) |
				(format.no_double_quotes << 1) |
				format.no_single_quotes;

	for (offs = 0, counter = 0, idx = 0; !(abacus & 128); idx++) {

		abacus	=	(delimiter ? ini_string[idx] == delimiter : is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_)) ?
						abacus & 159
					: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abacus | 64) ^ 32
					: !(abacus & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						((abacus & 223) | 64) ^ 16
					: !(abacus & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						((abacus & 223) | 64) ^ 8
					: ini_string[idx] ?
						(abacus & 223) | 64
					: 128;

		if (!(abacus & 88)) {

			if (!(abacus & 132)) {

				idx = ltrim_s(ini_string, idx, _LIBCONFINI_WITH_EOL_) - 1;

			}

			offs = ltrim_s(ini_string, offs, _LIBCONFINI_WITH_EOL_);

			if (f_foreach(ini_string, offs, rtrim_s(ini_string + offs, idx - offs, _LIBCONFINI_WITH_EOL_), counter++, format, user_data)) {

				return CONFINI_EFEINTR;

			}

			offs = idx + 1;

		}

	}

	return 0;

}


/**

	@brief			Removes spaces around all the delimiters of a stringified array
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members -- if zero (`INI_ANY_SPACE`) any
									space is delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@return			The new length of the string containing the array

	Out of quotes similar to ECMAScript `ini_string.replace(new RegExp("^\\s+|\\s*(?:(" + delimiter + ")\\s*|($))", "g"), "$1$2")`.
	If `INI_ANY_SPACE` (`0`) is used as delimiter, one or more different spaces (`/[\t \v\f\n\r]+/`) will
	always be collapsed to one space (' '), independently of their position.

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be used as well).

**/
size_t ini_collapse_array (char * const ini_string, const char delimiter, const IniFormat format) {

	uint16_t abacus;
	size_t iter;
	size_t idx, lshift;

	/*

	Mask `abacus` (12 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Any space is delimiter (const)
		FLAG_8		Unescaped single quotes are odd until now
		FLAG_16		Unescaped double quotes are odd until now
		FLAG_32		We are in an odd sequence of backslashes
		FLAG_64		We met a delimiter
		FLAG_128	This shift shall be without delimiter (possible just at the beginning or at the end of the buffer)
		FLAG_256	Remember this position
		FLAG_512	In this area a shift has to be done
		FLAG_1024	Account the shift now
		FLAG_2048	Continue the loop

	*/

	abacus	=	(delimiter ? 2176 : 2180) |
				(format.no_double_quotes << 1) |
				format.no_single_quotes;

	for (lshift = 0, iter = 0, idx = 0; abacus & 2048; idx++) {

		abacus	=	!(abacus & 28) && ini_string[idx] == delimiter ?

						(
							abacus & 64 ?
								(abacus & 2911) | 3904
							:
								((abacus & 2911) | 2880) ^ ((abacus >> 1) & 256)
						)

					: !(abacus & 24) && is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_) ?

						(
							abacus & 68 ?
								(abacus & 2911) | 2880
							:
								(abacus & 2975) | 2816
						) ^ ((abacus >> 1) & 256)

					: ini_string[idx] ?

						(
							(abacus & 512) && (abacus & 192) ?
								(abacus & 3263) | 3072
							:
								(abacus & 2239) | 2048
						) ^ (
							ini_string[idx] == _LIBCONFINI_BACKSLASH_ ? 32
							: !(abacus & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? 16
							: !(abacus & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? 8
							: abacus & 32
						)

					:

						((abacus & 607) | ((abacus << 1) & 1152)) ^ 128;

		if (abacus & 1024) {

			if (abacus & 128) {

				lshift += idx - iter;

			} else {

				ini_string[iter - lshift] = delimiter ? delimiter : _LIBCONFINI_SIMPLE_SPACE_;
				lshift += idx - iter - 1;

			}

			abacus &= 2943;

		}

		if (lshift) {

			ini_string[idx - lshift] = ini_string[idx];

		}

		if (abacus & 256) {

			iter = idx;

		}

	}

	for (iter = idx - lshift - 1; lshift; ini_string[idx - lshift--] = '\0');

	return iter;

}


/**

	@brief			Splits an INI array and calls a custom function for each member
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter between the array members -- if zero (`INI_ANY_SPACE`)
									any space is delimiter (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
	@param			format			The format of the INI file
	@param			f_foreach		The function that will be invoked for each array member
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be used as well).

	The function @p f_foreach will be invoked with five arguments: `member` (a pointer to @p ini_string),
	`memb_length` (the length of the member in bytes), `index` (the index of the member in number of
	members), `format` (the format of the INI file), `foreach_other` (the custom argument @p user_data
	previously passed). If @p f_foreach returns a non-zero value the function `ini_split_array()` will be
	interrupted.

**/
int ini_split_array (
	char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach) (
		char *member,
		size_t memb_length,
		size_t index,
		IniFormat format,
		void *foreach_other
	),
	void *user_data
) {

	uint8_t abacus;
	size_t offs, idx, counter;

	/* Mask `abacus` (8 bits used): as in `ini_array_get_length()` */

	abacus	=	(delimiter ? 4 : 0) |
				(format.no_double_quotes << 1) |
				format.no_single_quotes;

	for (offs = 0, counter = 0, idx = 0; !(abacus & 128); idx++) {

		abacus	=	(delimiter ? ini_string[idx] == delimiter : is_some_space(ini_string[idx], _LIBCONFINI_WITH_EOL_)) ?
						abacus & 159
					: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
						(abacus | 64) ^ 32
					: !(abacus & 42) && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
						((abacus & 223) | 64) ^ 16
					: !(abacus & 49) && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
						((abacus & 223) | 64) ^ 8
					: ini_string[idx] ?
						(abacus & 223) | 64
					: 128;

		if (!(abacus & 88)) {

			if (!(abacus & 132)) {

				idx = ltrim_h(ini_string, idx, _LIBCONFINI_WITH_EOL_) - 1;

			}

			ini_string[idx] = '\0';
			offs = ltrim_h(ini_string, offs, _LIBCONFINI_WITH_EOL_);

			if (f_foreach(ini_string + offs, rtrim_h(ini_string + offs, idx - offs, _LIBCONFINI_WITH_EOL_), counter++, format, user_data)) {

				return CONFINI_EFEINTR;

			}

			offs = idx + 1;

		}

	}

	return 0;

}


/**

	@brief			Checks whether a string matches *exactly* one of the booleans listed in
					the private constant `_LIBCONFINI_BOOLEANS_` (case insensitive)
	@param			ini_string			A string to be checked
	@param			return_value		A value that is returned if no matching boolean has been found
	@return			The matching boolean value (0 or 1) or @p return_value if no boolean has been found

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be used as well).

**/
signed int ini_get_bool (const char * const ini_string, const signed int return_value) {

	unsigned short int pair_idx, bool_idx, chr_idx;

	for (pair_idx = 0; pair_idx < sizeof(_LIBCONFINI_BOOLEANS_) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			for (chr_idx = 0; _LIBCONFINI_CHR_CASEFOLD_(ini_string[chr_idx]) == _LIBCONFINI_BOOLEANS_[pair_idx][bool_idx][chr_idx]; chr_idx++) {

				if (!ini_string[chr_idx]) {

					return bool_idx;

				}

			}

		}

	}

	return return_value;

}


/**

	@brief			Checks whether the first letter of a string matches the first letter of one of the
					booleans listed in the private constant `_LIBCONFINI_BOOLEANS_` (case insensitive)
	@param			ini_string			A string to be checked
	@param			return_value		A value that is returned if no matching boolean has been found
	@return			The matching boolean value (0 or 1) or @p return_value if no boolean has been found

	Usually @p ini_string comes from an `IniDispatch` (but any other string may be used as well).

**/
signed int ini_get_lazy_bool (const char * const ini_string, const signed int return_value) {

	unsigned short int pair_idx, bool_idx;

	for (pair_idx = 0; pair_idx < sizeof(_LIBCONFINI_BOOLEANS_) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			if (_LIBCONFINI_CHR_CASEFOLD_(*ini_string) == *_LIBCONFINI_BOOLEANS_[pair_idx][bool_idx]) {

				return bool_idx;

			}

		}

	}

	return return_value;

}



/* LINKS -- In case you don't want to `#include <stdlib.h>` in your source */

int (* const ini_get_int) (const char *ini_string) = &atoi;

long int (* const ini_get_lint) (const char *ini_string) = &atol;

long long int (* const ini_get_llint) (const char *ini_string) = &atoll;

double (* const ini_get_float) (const char *ini_string) = &atof;



/* VARIABLES */

int INI_INSENSITIVE_LOWERCASE = _LIBCONFINI_TRUE_;
char *INI_IMPLICIT_VALUE = (char *) 0;
size_t INI_IMPLICIT_V_LEN = 0;



/* EOF */

