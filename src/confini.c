/*	-*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-	*/

/**

	@file		confini.c
	@brief		libconfini functions
	@author		Stefano Gioffr&eacute;
	@copyright 	GNU Public License v3
	@date		2016

**/

#include <stdio.h>
#include <stdlib.h>
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
					If set to `1`, key- and section- names will not be rendered in lower case
	@property	IniFormat::no_spaces_in_names
					If set to `1`, key- and section- names containing spaces will be rendered as `#INI_UNKNOWN`.
					Note that having `INI_ANY_SPACE` as delimiter will not set this option to `1`: spaces will
					be still allowed in section names.					
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
					The size of the parsed file in members (nodes)


	@struct		IniDispatch

	@property	IniDispatch::format
					The format of the INI file (see struct `#IniFormat`)
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
#define _LIBCONFINI_BYTE_ unsigned char

/* _LIBCONFINI_SIZE_ must be able to express at least the size of an INI file */
#define _LIBCONFINI_SIZE_ unsigned long int

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



/* FUNCTIONAL CONSTANTS */

/**

	@brief A list of possible string representations of boolean pairs

	There can be infinite pairs here, but be aware of the lazy behavior of ini_get_lazy_bool().
	Everything lowercase in this list!

**/
static const char * const _LIBCONFINI_BOOLEANS_[][2] = {
	{ "no", "yes" },
	{ "false", "true" },
	{ "0", "1" }
};

/*
	This can be any character, in theory... But after the left-trim of each line a leading
	space works pretty well as metacharacter.
*/
#define _LIBCONFINI_INLINE_MARKER_ '\t'



/* ABSTRACT UTILITIES */

static const char _LIBCONFINI_SPACES_[] = { _LIBCONFINI_SIMPLE_SPACE_, '\t', '\v', '\f', _LIBCONFINI_LF_, _LIBCONFINI_CR_ };

/* Possible lengths of array #_LIBCONFINI_SPACES_ */
#define _LIBCONFINI_JUST_S_T_ 2
#define _LIBCONFINI_NO_EOL_ 4
#define _LIBCONFINI_WITH_EOL_ 6

static inline _LIBCONFINI_BOOL_ is_some_space (const char ch, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_BYTE_ idx;
	for (idx = 0; idx < depth && ch != _LIBCONFINI_SPACES_[idx]; idx++);
	return idx < depth;
}


/**

	@brief			Soft left trim -- does not change the buffer
	@param			lt_s			The target string
	@param			start_from		The offset where to start the left trim
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The offset of the first non-space character

**/
static inline _LIBCONFINI_SIZE_ ltrim_s (const char * const lt_s, const _LIBCONFINI_SIZE_ start_from, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_SIZE_ lt_i = start_from;
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
static inline _LIBCONFINI_SIZE_ ltrim_h (char * const lt_s, const _LIBCONFINI_SIZE_ start_from, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_SIZE_ lt_i = start_from;
	for (; lt_s[lt_i] && is_some_space(lt_s[lt_i], depth); lt_s[lt_i++] = '\0');
	return lt_i;
}


/**

	@brief			Unescaped hard left trim (left trim of `/^(\s+|\\[\n\r])+/`) -- **does** change the buffer
	@param			ult_s			The target string
	@param			start_from		The offset where to start the left trim
	@return			The offset of the first non matching character

**/
static inline _LIBCONFINI_SIZE_ ultrim_h (char * const ult_s, const _LIBCONFINI_SIZE_ start_from) {

	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_SIZE_ ult_i = start_from;

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
	@param			depth			What is actually considered a space -- a subset of @link #_LIBCONFINI_SPACES_ @endlink ranging from 1 to 6
	@return			The length of the string until the last non-space character

**/
static inline _LIBCONFINI_SIZE_ rtrim_s (const char * const rt_s, const _LIBCONFINI_SIZE_ length, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_SIZE_ rt_l = length;
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
static inline _LIBCONFINI_SIZE_ rtrim_h (char * const rt_s, const _LIBCONFINI_SIZE_ length, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_SIZE_ rt_l = length;
	for (; rt_l > 0 && is_some_space(rt_s[rt_l - 1], depth); rt_s[--rt_l] = '\0');
	return rt_l;
}


/**

	@brief			Unescaped soft right trim (right trim of `/(\s+|\\[\n\r])+$/`) -- does not change the buffer
	@param			urt_s			The target string
	@param			length			The length of the string
	@return			The length of the string until the last non-space character

**/
static inline _LIBCONFINI_SIZE_ urtrim_s (const char * const urt_s, const _LIBCONFINI_SIZE_ length) {

	_LIBCONFINI_SIZE_ urt_l = length;

	/*

	Mask `abacus` (2 bits used):

		FLAG_1		Continue the loop
		FLAG_2		Next character is a new line character

	*/

	for (

		_LIBCONFINI_BYTE_ abacus = 1;

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

	@brief			Converts a string to lower case
	@param			string		The target string
	@return			Nothing

**/
static inline void string_tolower (char * const str) {
	for (char *ch_ptr = str; *ch_ptr; ++ch_ptr) {
		*ch_ptr = *ch_ptr > 0x40 && *ch_ptr < 0x5b ? *ch_ptr | 0x60 : *ch_ptr;
	}
}



/* CONCRETE UTILITIES */


/**

	@brief			Checks whether a character can represent the opening of a disabled entry
					within a given format
	@param			ch			The character to check
	@param			format		The format of the INI file
	@return			1 if @p ch matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_parsable_char (const char ch, const IniFormat format) {
	return (format.semicolon == INI_PARSE_COMMENT && ch == _LIBCONFINI_SEMICOLON_) || (format.hash == INI_PARSE_COMMENT && ch == _LIBCONFINI_HASH_);
}


/**

	@brief			Checks whether a character can represent the opening of a comment within
					a given format
	@param			ch			The character to check
	@param			format		The format of the INI file
	@return			1 if @p ch matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_comm_char (const char ch, const IniFormat format) {
	return (format.semicolon != INI_NORMAL_CHARACTER && ch == _LIBCONFINI_SEMICOLON_) || (format.hash != INI_NORMAL_CHARACTER && ch == _LIBCONFINI_HASH_);
}


/**

	@brief			Checks whether a character can represent the opening of a comment that
					has to be ignored within a given format
	@param			ch			The character to check
	@param			format		The format of the INI file
	@return			1 if @p ch matches, 0 otherwise

**/
static inline _LIBCONFINI_BOOL_ is_erase_char (const char ch, const IniFormat format) {
	return (format.semicolon == INI_FORGET_COMMENT && ch == _LIBCONFINI_SEMICOLON_) || (format.hash == INI_FORGET_COMMENT && ch == _LIBCONFINI_HASH_);
}


/**

	@brief			Gets the position of the first delimiter out of quotes
	@param			string		The string to analyse
	@param			length		The length of the string
	@param			format		The format of the INI file
	@return			The offset of the delimiter or @p length if the delimiter has not
					been not found

**/
static inline _LIBCONFINI_SIZE_ get_delimiter_pos (const char * const str, const _LIBCONFINI_SIZE_ len, const IniFormat format) {

	_LIBCONFINI_SIZE_ idx = 0;

	/*

	Mask `abacus` (5 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		We are in an odd sequence of backslashes

	*/

	for (

		_LIBCONFINI_BYTE_ abacus = (format.no_double_quotes << 1) | format.no_single_quotes;

			idx < len && (
				(abacus & 12) || !(
					format.delimiter_symbol ?
						str[idx] == format.delimiter_symbol
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
	@param			string			Target string
	@param			length			Length of the string
	@param			is_disabled		The string represents a disabled entry
	@return			Number of characters removed

**/
static _LIBCONFINI_SIZE_ unescape_cr_lf (char * const nstr, const _LIBCONFINI_SIZE_ len, const _LIBCONFINI_BOOL_ is_disabled, const IniFormat format) {

	_LIBCONFINI_SIZE_ idx, iter, lshift;
	_LIBCONFINI_BOOL_ is_escaped = _LIBCONFINI_FALSE_;
	_LIBCONFINI_BYTE_ cr_or_lf = 5;

	for (is_escaped = _LIBCONFINI_FALSE_, idx = 0, lshift = 0; idx < len; idx++) {

		if (is_escaped && (nstr[idx] == _LIBCONFINI_SPACES_[cr_or_lf] || nstr[idx] == _LIBCONFINI_SPACES_[cr_or_lf ^= 1])) {

			for (

				iter = idx,
				idx += nstr[idx + 1] == _LIBCONFINI_SPACES_[cr_or_lf ^ 1];

					iter < idx + 1;

				nstr[iter - lshift - 1] = nstr[iter],
				iter++

			);

			lshift++;

			if (is_disabled) {

				iter = ltrim_s(nstr, iter, _LIBCONFINI_NO_EOL_);

				if (is_parsable_char(nstr[iter], format)) {

					lshift += iter - idx;
					idx = iter;

				}

			}

			is_escaped = _LIBCONFINI_FALSE_;

		} else {

			is_escaped = nstr[idx] == _LIBCONFINI_BACKSLASH_ ? !is_escaped : _LIBCONFINI_FALSE_;

			if (lshift) {

				nstr[idx - lshift] = nstr[idx];

			}

		} 

	}

	for (idx = len - lshift; idx < len; nstr[idx++] = '\0');

	return len - lshift;

}


/**

	@brief			Out of quotes similar to ECMAScript `string.replace(/^[\n\r]\s*|(\s)+/g, " ")`
	@param			string		The string to collapse
	@param			format		The format of the INI file
	@return			The new length of the string

**/
static _LIBCONFINI_SIZE_ collapse_spaces (char * const str, const IniFormat format) {

	/*

	Mask `abacus` (6 bits used):

		FLAG_1		Single quotes are not metacharacters (const)
		FLAG_2		Double quotes are not metacharacters (const)
		FLAG_4		Unescaped single quotes are odd until now
		FLAG_8		Unescaped double quotes are odd until now
		FLAG_16		We are in an odd sequence of backslashes
		FLAG_32		More than one space is here

	*/

	_LIBCONFINI_BYTE_	abacus	=	(is_some_space(*str, _LIBCONFINI_WITH_EOL_) ? 32 : 0)
									| (format.no_double_quotes << 1)
									| format.no_single_quotes;

	_LIBCONFINI_SIZE_ idx, lshift;

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
	@param			section_name		The target string
	@param			format				The format of the INI file
	@return			The new length of the string

	Out of quotes, similar to ECMAScript `section_name.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/\s+/g, " ")`

	A section name can start with a dot (append), but cannot end with a dot. Spaces sorrounding dots
	will be removed. Single quotes or double quotes (if active and present) prevent changes.

**/
static _LIBCONFINI_SIZE_ sanitize_section_name (char * const str, const IniFormat format) {

	_LIBCONFINI_SIZE_ idx, lshift;
	unsigned int abacus;

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

	abacus	=	(is_some_space(*str, _LIBCONFINI_WITH_EOL_) ? 256 : 32)
				| (format.no_double_quotes << 1)
				| format.no_single_quotes;

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

	For multiline comments: `commstr.replace(/(^|\n\r?|\r\n?)[ \t\v\f]*[#;]+/g, "$1")`
	For single line comments: `commstr.replace(/^[ \t\v\f]*[#;]+/, "")`

**/
static _LIBCONFINI_SIZE_ uncomment (char * const commstr, _LIBCONFINI_SIZE_ len, const IniFormat format) {

	_LIBCONFINI_SIZE_ lshift, idx;

	if (format.multiline_entries) {

		/* The comment is not multiline */

		for (lshift = 0; lshift < len && is_comm_char(commstr[lshift], format); lshift++);

		if (!lshift) {

			return len;

		}

		for (idx = lshift; idx < len; commstr[idx - lshift] = commstr[idx], idx++);

	} else {

		/* The comment is multiline */

		_LIBCONFINI_BYTE_ abacus;

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
						: !(abacus & 32) && is_comm_char(commstr[idx], format) ?
							(abacus & 40) | 8
						: !(abacus & 40) && is_some_space(commstr[idx], _LIBCONFINI_NO_EOL_)?
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
	@param			string				String containing an individual node
	@param			length				Length of the node
	@param			wanna_be_active		A boolean: TRUE if we are in an disabled node, FALSE otherwise
	@param			format				The format of the INI file
	@return			The node type (see header)

**/
static _LIBCONFINI_BYTE_ get_type_as_active (
	const char * const nodestr,
	const _LIBCONFINI_SIZE_ len,
	const _LIBCONFINI_BOOL_ wanna_be_active,
	const IniFormat format
) {

	if (!len || *nodestr == format.delimiter_symbol || is_comm_char(*nodestr, format)) {

		return INI_UNKNOWN;

	}

	unsigned int abacus;
	_LIBCONFINI_SIZE_ idx;

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
								(abacus ^ 1) | (((abacus << 1) & 2) & (~abacus & 2))
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
							: abacus & 15;;

			}

		}

		return INI_UNKNOWN;

	}

	/* It can be just a key... */

	idx = get_delimiter_pos(nodestr, len, format);

	if (wanna_be_active && !format.disabled_can_be_implicit && idx == len) {

		return INI_UNKNOWN;

	}

	if (format.no_spaces_in_names && format.delimiter_symbol) {

		idx = urtrim_s(nodestr, idx) - 1;

		do {

			if (is_some_space(nodestr[idx], _LIBCONFINI_WITH_EOL_)) {

				return INI_UNKNOWN;

			}

		} while (idx-- > 0);

	}

	return INI_KEY;

}


/**

	@brief			Recursive function, examines a (single-/multi- line) segment and searches for sub-segments.
	@param			segment		Segment to examine
	@param			format		The format of the INI file
	@return			Number of sub-segments found, included itself

**/
static _LIBCONFINI_SIZE_ further_cuts (char * const segment, const IniFormat format) {

	if (!*segment) { return 0; }

	_LIBCONFINI_BOOL_ show_comments = !is_erase_char(*segment, format);
	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_SIZE_ idx, focus_at, search_at = 0, unparse_at = 0, fragm_size = show_comments;

	/*

	Mask `abacus` (7 bits used):

		FLAG_1		We are in an odd sequence of backslashes
		FLAG_2		Unescaped single quotes are odd until now
		FLAG_4		Unescaped double quotes are odd until now
		FLAG_8		This does not immediately follow `[\n\r]\s*`
		FLAG_16		The previous character was a space
		FLAG_32		This can be a disabled entry
		FLAG_64		This is nothing but a (multiline) comment

	*/

	abacus		=	is_parsable_char(*segment, format) && !(
						format.no_disabled_after_space && is_some_space(segment[1], _LIBCONFINI_NO_EOL_)
					) ?
						46
					: !format.multiline_entries && (
						is_comm_char(*segment, format) || *segment == _LIBCONFINI_INLINE_MARKER_
					) ?
						78
					:
						14;

	if (abacus & 96) {

		for (idx = ltrim_s(segment, 1, _LIBCONFINI_NO_EOL_); segment[idx]; idx++) {

			if (is_comm_char(segment[idx], format)) {

				/* Search for inline comments in (supposedly) disabled items */

				if (abacus == 62) {

					focus_at = ltrim_s(segment, 1, _LIBCONFINI_NO_EOL_);

					if (get_type_as_active(segment + focus_at, idx - focus_at, _LIBCONFINI_TRUE_, format)) {

						segment[idx] = _LIBCONFINI_INLINE_MARKER_;
						segment[idx - 1] = '\0';

						if (show_comments) {

							fragm_size++;

						}

						if (format.multiline_entries) {

							unparse_at = idx + 1;
							break;

						}

					} else if (format.multiline_entries) {

						unparse_at = 1;
						break;

					}

				}

				abacus &= 102;
				abacus |= 8;

			} else if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

				/* Search for `/\\(?:\n\r?|\r\n?)\s*[^;#]/` in multiline disabled items */

				idx = ltrim_s(segment, idx + 1, _LIBCONFINI_WITH_EOL_);

				if (
						(
							format.no_disabled_after_space && is_some_space(segment[idx + 1], _LIBCONFINI_NO_EOL_)
						) || !(
							(
								is_comm_char(segment[idx], format) && !format.multiline_entries
							) || (
								is_parsable_char(segment[idx], format) && format.multiline_entries < 2
							)
						)
					) {

					rtrim_h(segment, idx, _LIBCONFINI_WITH_EOL_);

					if (segment[idx]) {

						search_at = idx + 1;

						if (show_comments) {

							fragm_size++;

						}

					}

					break;

				}

				abacus &= 102;

			} else if (is_some_space(segment[idx], _LIBCONFINI_NO_EOL_)) {

				abacus &= 102;
				abacus |= 24;

			} else {

				abacus	=	!(abacus & 3) && !format.no_double_quotes && segment[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ?
								abacus ^ 4
							: !(abacus & 5) && !format.no_single_quotes && segment[idx] == _LIBCONFINI_SINGLE_QUOTES_ ?
								abacus ^ 2
							: segment[idx] == _LIBCONFINI_BACKSLASH_ ?
								abacus ^ 1
							:
								abacus & 126;

				abacus &= 111;
				abacus |= 8;

			}

		}

		if (format.multiline_entries && !unparse_at) {

			focus_at = ltrim_s(segment, 1, _LIBCONFINI_NO_EOL_);

			if (segment[focus_at] && !get_type_as_active(segment + focus_at, idx - focus_at, _LIBCONFINI_TRUE_, format)) {

				unparse_at = 1;
			}

		}

	} else if (is_comm_char(*segment, format)) {

		unparse_at = 1;

	} else {

		search_at = 1;

	}

	if (unparse_at) {

		/* Cut unparsable multiline comments */

		cut_unparsable:

		for (idx = unparse_at; segment[idx]; idx++) {

			if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

				fragm_size += further_cuts(segment + ultrim_h(segment, idx), format);
				break;

			}

		}

	}

	if (search_at) {

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

		for (abacus = 96 | (format.no_double_quotes << 1) | format.no_single_quotes, idx = search_at; segment[idx]; idx++) {

			abacus	=	is_comm_char(segment[idx], format) ?
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

				segment[idx] = _LIBCONFINI_INLINE_MARKER_;
				segment[idx - 1] = '\0';
				search_at = 0;

				if (format.multiline_entries) {

					if (show_comments) {

						fragm_size++;

					}

					unparse_at = idx + 1;
					goto cut_unparsable;

				}

				fragm_size += further_cuts(segment + idx, format);
				break;

			}

		}

	}

	return fragm_size;

}


/*----------------------------------------------------*/



		/*\
		|*|
		|*|   GLOBAL ENVIRONMENT
		|*|
		\*/



/* LIBRARY'S MAIN FUNCTION */

/**

	@brief			Parses an INI file and dispatches its content
	@param			path			The path of the INI file
	@param			format			The format of the INI file
	@param			f_init			The function that will be invoked before the dispatch, or NULL
	@param			f_foreach		The function that will be invoked for each dispatch, or NULL
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

**/
unsigned int load_ini_file (
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

	FILE * const config_file = fopen(path, "r");

	if (config_file == NULL) {

		return CONFINI_ENOENT;

	}

	_LIBCONFINI_SIZE_ len;

	fseek(config_file, 0, SEEK_END);
	len = ftell(config_file);
	rewind(config_file);

	char * const cache = (char *) malloc((len + 1) * sizeof(char));

	if (cache == NULL) {

		return CONFINI_ENOMEM;

	}

	unsigned int return_value = 0;

	if (fread(cache, sizeof(char), len, config_file) < len) {

		return_value = CONFINI_EIO;
		goto free_and_exit;

	}

	fclose(config_file);
	cache[len] = '\0';

	_LIBCONFINI_BOOL_ tmp_bool;
	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_SIZE_ tmp_uint, idx, node_at;

	/* PART ONE: Examine and isolate each segment */

	#define _LIBCONFINI_IS_ESCAPED_ tmp_bool
	#define _LIBCONFINI_NL_ID_ abacus
	#define _LIBCONFINI_MEMBERS_ tmp_uint

	for (

		_LIBCONFINI_MEMBERS_ = 0,
		_LIBCONFINI_NL_ID_ = 5,
		_LIBCONFINI_IS_ESCAPED_ = _LIBCONFINI_FALSE_,
		node_at = 0,
		idx = 0;

			idx < len;

		idx++

	) {

		if (cache[idx] == _LIBCONFINI_SPACES_[_LIBCONFINI_NL_ID_] || cache[idx] == _LIBCONFINI_SPACES_[_LIBCONFINI_NL_ID_ ^= 1]) {

			if (format.multiline_entries < 3 && _LIBCONFINI_IS_ESCAPED_) {

				idx += cache[idx + 1] == _LIBCONFINI_SPACES_[_LIBCONFINI_NL_ID_ ^ 1];

			} else {

				cache[idx] = '\0';
				_LIBCONFINI_MEMBERS_ += further_cuts(cache + ultrim_h(cache, node_at), format);
				node_at = idx + 1;

			}

			_LIBCONFINI_IS_ESCAPED_ = _LIBCONFINI_FALSE_;

		} else if (cache[idx] == _LIBCONFINI_BACKSLASH_) {

			_LIBCONFINI_IS_ESCAPED_ = !_LIBCONFINI_IS_ESCAPED_;

		} else {

			/* Replace `NUL` characters in the buffer (if any) with simple spaces */

			if (!cache[idx]) {

				cache[idx] = _LIBCONFINI_SIMPLE_SPACE_;

			}

			_LIBCONFINI_IS_ESCAPED_ = _LIBCONFINI_FALSE_;

		}

	}

	IniStatistics this_doc = {
		.format = format,
		.bytes = len,
		.members = _LIBCONFINI_MEMBERS_
	};

	#undef _LIBCONFINI_MEMBERS_
	#undef _LIBCONFINI_NL_ID_
	#undef _LIBCONFINI_IS_ESCAPED_

	/* Debug */
	/*

	for (_LIBCONFINI_SIZE_ tmp = 0; tmp < len; tmp++) {
		putchar(cache[tmp] > 0 ? cache[tmp] : '$');
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

	#define _LIBCONFINI_PARENT_IS_DISABLED_ tmp_bool

	_LIBCONFINI_SIZE_ parse_at, real_parent_len = 0, curr_parent_len = 0;
	char *curr_parent_str = cache + len, *subparent_str = curr_parent_str, *real_parent_str = curr_parent_str;
	IniDispatch this_d = {
		.format = format,
		.dispatch_id = 0
	};

	_LIBCONFINI_PARENT_IS_DISABLED_ = _LIBCONFINI_FALSE_;

	for (node_at = 0, idx = 0; idx <= len; idx++) {

		if (cache[idx]) {

			continue;

		}

		if (!cache[node_at] || is_erase_char(cache[node_at], format)) {

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

			Mask `abacus` (3 bits used):

				FLAG_1		Continue the loop
				FLAG_2		We are not in `\n[ \t\v\f]*`
				FLAG_4		This is a space and FLAG_2 == TRUE

			*/

			for (abacus = 1, tmp_uint = 1; abacus && tmp_uint < this_d.d_len; tmp_uint++) {

				abacus	=	this_d.data[tmp_uint] == _LIBCONFINI_LF_ || this_d.data[tmp_uint] == _LIBCONFINI_CR_ ? 1
							: (abacus & 4) && is_comm_char(this_d.data[tmp_uint], format) ? 0
							: (abacus & 2) && is_some_space(this_d.data[tmp_uint], _LIBCONFINI_NO_EOL_) ? 7
							: 3;

			}

			this_d.type		=	tmp_uint == this_d.d_len ?
									get_type_as_active(cache + parse_at, idx - parse_at, _LIBCONFINI_TRUE_, format)
								:
									0;

			if (this_d.type) {

				this_d.data = cache + parse_at;
				this_d.d_len = idx - parse_at;

			}

			this_d.type |= 4;

		} else if (is_comm_char(*this_d.data, format)) {

			this_d.type = INI_COMMENT;

		} else if (*this_d.data == _LIBCONFINI_INLINE_MARKER_) {

			this_d.type = INI_INLINE_COMMENT;

		} else {

			this_d.type = get_type_as_active(this_d.data, this_d.d_len, _LIBCONFINI_FALSE_, format);

		}

		if (curr_parent_len && *subparent_str) {

			tmp_uint = 0;

			do {

				curr_parent_str[tmp_uint + curr_parent_len] = subparent_str[tmp_uint];

			} while (subparent_str[tmp_uint++]);

			curr_parent_len += tmp_uint - 1;
			subparent_str = curr_parent_str + curr_parent_len;

		}

		if (_LIBCONFINI_PARENT_IS_DISABLED_ && !(this_d.type & 4)) {

			real_parent_str[real_parent_len] = '\0';
			curr_parent_len = real_parent_len;
			curr_parent_str = real_parent_str;
			_LIBCONFINI_PARENT_IS_DISABLED_ = _LIBCONFINI_FALSE_;

		} else if (!_LIBCONFINI_PARENT_IS_DISABLED_ && this_d.type == INI_DISABLED_SECTION) {

			real_parent_len = curr_parent_len;
			real_parent_str = curr_parent_str;
			_LIBCONFINI_PARENT_IS_DISABLED_ = _LIBCONFINI_TRUE_;

		}

		this_d.append_to = curr_parent_str;
		this_d.at_len = curr_parent_len;

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

					abacus = (format.no_double_quotes << 1) | format.no_single_quotes, tmp_uint = 0;

						this_d.data[tmp_uint] && (
							(abacus & 12) || this_d.data[tmp_uint] != _LIBCONFINI_CLOSE_SECTION_
						);

					abacus	=	this_d.data[tmp_uint] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 16
								: !(abacus & 22) && this_d.data[tmp_uint] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 8
								: !(abacus & 25) && this_d.data[tmp_uint] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 4
								: abacus & 15,

					tmp_uint++

				);

				while (this_d.data[tmp_uint]) {

					this_d.data[tmp_uint++] = '\0';

				}

				this_d.d_len = sanitize_section_name(this_d.data, format);

				if (curr_parent_len && *this_d.data == _LIBCONFINI_SUBSECTION_) {

					subparent_str = this_d.data;

				} else {

					curr_parent_str = this_d.data + (!curr_parent_len && *this_d.data == _LIBCONFINI_SUBSECTION_);
					curr_parent_len = this_d.d_len - (!curr_parent_len && *this_d.data == _LIBCONFINI_SUBSECTION_);
					subparent_str = cache + idx;
					this_d.append_to = subparent_str;
					this_d.at_len = 0;

				}

				if (!format.case_sensitive) {

					string_tolower(this_d.data);

				}

				break;

			case INI_KEY:
			case INI_DISABLED_KEY:

				tmp_uint = get_delimiter_pos(this_d.data, this_d.d_len, format);

				if (this_d.d_len && tmp_uint && tmp_uint < this_d.d_len) {

					this_d.data[tmp_uint] = '\0';

					if (format.do_not_collapse_values) {

						this_d.value = this_d.data + ltrim_h(this_d.data, tmp_uint + 1, _LIBCONFINI_WITH_EOL_);
						this_d.v_len = rtrim_h(this_d.value, this_d.d_len + this_d.data - this_d.value, _LIBCONFINI_WITH_EOL_);

					} else {

						this_d.value = this_d.data + tmp_uint + 1;
						this_d.v_len = collapse_spaces(this_d.value, format);

					}

				} else if (format.implicit_is_not_empty) {

					this_d.value = INI_IMPLICIT_VALUE;
					this_d.v_len = INI_IMPLICIT_V_LEN;
				}

				this_d.d_len = collapse_spaces(this_d.data, format);

				if (!format.case_sensitive) {

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

	#undef _LIBCONFINI_PARENT_IS_DISABLED_

	free(cache);
	return return_value;

}



/* FUNCTIONS FOR THE USER (NOT USED BY LIBCONFINI) */


/**

	@brief			Sets the valued used for implicit keys
	@param			implicit_value		The string to be used as implicit value (usually `"YES"`, or `"TRUE"`)
	@param			implicit_v_len		The length of @p implicit_value (usually `0`, independently of its real length)
	@return			Nothing

**/
void ini_set_implicit_value (char * const implicit_value, const unsigned long int implicit_v_len) {

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

	#define _LIBCONFINI_READ_FORMAT_ID_ID_(SIZE, PROPERTY, IGNORE_ME) \
		mask |= source.PROPERTY << bitpos;\
		bitpos += SIZE;

	_LIBCONFINI_EXPAND_MODEL_FORMAT_AS_(_LIBCONFINI_READ_FORMAT_ID_ID_)

	#undef _LIBCONFINI_READ_FORMAT_ID_ID_

	return mask;

}


/**

	@brief			Sets all the values of an IniFormat by reading them from an ::IniFormatId
	@param			dest_format		The IniFormat to be set
	@param			mask			The `#IniFormatId` to be read
	@return			Nothing

**/
void ini_format_set_to_id (IniFormat *dest_format, IniFormatId format_id) {

	#define _LIBCONFINI_MAX_1_BITS_ 1
	#define _LIBCONFINI_MAX_2_BITS_ 3
	#define _LIBCONFINI_MAX_3_BITS_ 7
	#define _LIBCONFINI_MAX_4_BITS_ 15
	#define _LIBCONFINI_MAX_5_BITS_ 31
	#define _LIBCONFINI_MAX_6_BITS_ 63
	#define _LIBCONFINI_MAX_7_BITS_ 127
	#define _LIBCONFINI_MAX_8_BITS_ 255
	#define _LIBCONFINI_READ_FORMAT_ID_(SIZE, PROPERTY, IGNORE_ME) \
		dest_format->PROPERTY = format_id & _LIBCONFINI_MAX_##SIZE##_BITS_;\
		format_id >>= SIZE;

	_LIBCONFINI_EXPAND_MODEL_FORMAT_AS_(_LIBCONFINI_READ_FORMAT_ID_)

	#undef _LIBCONFINI_READ_FORMAT_ID_
	#undef _LIBCONFINI_MAX_8_BITS_
	#undef _LIBCONFINI_MAX_7_BITS_
	#undef _LIBCONFINI_MAX_6_BITS_
	#undef _LIBCONFINI_MAX_5_BITS_
	#undef _LIBCONFINI_MAX_4_BITS_
	#undef _LIBCONFINI_MAX_3_BITS_
	#undef _LIBCONFINI_MAX_2_BITS_
	#undef _LIBCONFINI_MAX_1_BITS_

}


/**

	@brief			Unescapes `\\`, `\'` and `\"` and removes all unescaped quotes (if single/double quotes are active)
	@param			ini_string		The string to be unescaped
	@param			format			The format of the INI file
	@return			The new length of the string

**/
unsigned long int ini_unquote (char * const ini_string, const IniFormat format) {

	_LIBCONFINI_BYTE_ qmask;
	_LIBCONFINI_SIZE_ lshift, nbacksl, idx;

	/*

	Mask `qmask` (4 bits used):

		FLAG_1		Unescaped single quotes are even until now
		FLAG_2		Unescaped double quotes are even until now
		FLAG_4		This is an unescaped single quote and format supports single quotes
		FLAG_8		This is an unescaped double quote and format supports double quotes

	*/

	for (qmask = 3, lshift = 0, nbacksl = 0, idx = 0; ini_string[idx]; idx++) {

		qmask	=	(qmask & 3) | (
						!format.no_double_quotes && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? 8
						: !format.no_single_quotes && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? 4
						: 0
					);

		if (!(nbacksl & 1) && !((~qmask & 9) && (~qmask & 6))) {

			qmask ^= qmask >> 2;
			lshift++;
			continue;

		}

		if (ini_string[idx] == _LIBCONFINI_BACKSLASH_) {

			nbacksl++;

		} else {

			if ((nbacksl & 1) && (qmask & 12)) {

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
	@param			delimiter		The delimiter of the array members
	@param			format			The format of the INI file
	@return			The length of the INI array

**/
unsigned long int ini_array_get_length (const char * const ini_string, const char delimiter, const IniFormat format) {

	unsigned long int arr_length = 0;
	_LIBCONFINI_SIZE_ idx;
	_LIBCONFINI_BYTE_ abacus;

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

	abacus	=	(delimiter ? 4 : 0)
				| (format.no_double_quotes << 1)
				| format.no_single_quotes;

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

	@brief			Calls a custom function for each member of an INI array -- useful for read-only (const) stringified arrays
	@param			ini_string			The stringified array
	@param			delimiter			The delimiter of the array members
	@param			format				The format of the INI file
	@param			f_foreach			The function that will be invoked for each array member
	@param			user_data			A custom argument, or NULL
	@return			Zero for success, otherwise an error code

**/
unsigned int ini_array_foreach (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach) (
		const char *member,
		unsigned int offset,
		unsigned int memb_length,
		unsigned int index,
		IniFormat format,
		void *user_data
	),
	void *user_data
) {

	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_SIZE_ idx, offs, counter;

	/* Mask `abacus` (8 bits used): as in `ini_array_get_length()` */

	abacus	=	(delimiter ? 4 : 0)
				| (format.no_double_quotes << 1)
				| format.no_single_quotes;

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
	@param			ini_string			The stringified array
	@param			delimiter			The delimiter of the array members
	@param			format				The format of the INI file
	@return			The new length of the string containing the array

	Out of quotes similar to ECMAScript `ini_string.replace(new RegExp("^\\s+|\\s*(?:(" + delimiter + ")\\s*|($))", "g"), "$1$2")`.
	If `INI_ANY_SPACE` (`0`) is used as delimiter one or more different spaces (`/[\t \v\f\n\r]+/`) will always be
	collapsed to one space (' '), independently of their position.

**/
unsigned long int ini_collapse_array (char * const ini_string, const char delimiter, const IniFormat format) {

	unsigned int abacus;
	unsigned long int iter;
	_LIBCONFINI_SIZE_ idx, lshift;

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

	abacus	=	(delimiter ? 2176 : 2180)
				| (format.no_double_quotes << 1)
				| format.no_single_quotes;

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

	@brief			Splits an INI array and calls a custom function for each member.
	@param			ini_string		The stringified array
	@param			delimiter		The delimiter of the array members
	@param			format			The format of the INI file
	@param			f_foreach		The function that will be invoked for each array member
	@param			user_data		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

**/
unsigned int ini_split_array (
	char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach) (
		char *member,
		unsigned int memb_length,
		unsigned int index,
		IniFormat format,
		void *user_data
	),
	void *user_data
) {

	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_SIZE_ offs, idx, counter;

	/* Mask `abacus` (8 bits used): as in `ini_array_get_length()` */

	abacus	=	(delimiter ? 4 : 0)
				| (format.no_double_quotes << 1)
				| format.no_single_quotes;

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
					the private constant @link #_LIBCONFINI_BOOLEANS_ @endlink (case insensitive)
	@param			ini_string			A string to be checked
	@param			return_value		A value that is returned if no matching boolean has been found
	@return			The matching boolean value (0 or 1) or @p return_value if no boolean has been found

**/
signed int ini_get_bool (const char * const ini_string, const signed int return_value) {

	unsigned short int pair_idx, bool_idx, chr_idx;

	for (pair_idx = 0; pair_idx < sizeof(_LIBCONFINI_BOOLEANS_) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			for (

				chr_idx = 0;

					(
						ini_string[chr_idx] > 0x40 && ini_string[chr_idx] < 0x5b ?
							ini_string[chr_idx] | 0x60
						:
							ini_string[chr_idx]
					) == _LIBCONFINI_BOOLEANS_[pair_idx][bool_idx][chr_idx];

				chr_idx++

			) {

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
					booleans listed in the private constant @link #_LIBCONFINI_BOOLEANS_ @endlink (case insensitive)
	@param			ini_string			A string to be checked
	@param			return_value		A value that is returned if no matching boolean has been found
	@return			The matching boolean value (0 or 1) or @p return_value if no boolean has been found

**/
signed int ini_get_lazy_bool (const char * const ini_string, const signed int return_value) {

	unsigned short int pair_idx, bool_idx;
	const char ch = *ini_string > 0x40 && *ini_string < 0x5b ? *ini_string | 0x60 : *ini_string;

	for (pair_idx = 0; pair_idx < sizeof(_LIBCONFINI_BOOLEANS_) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			if (ch == *_LIBCONFINI_BOOLEANS_[pair_idx][bool_idx]) {

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

char *INI_IMPLICIT_VALUE = (char *) 0;
unsigned long int INI_IMPLICIT_V_LEN = 0;



/* EOF */

