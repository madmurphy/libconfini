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

	@var		IniFormat::delimiter_symbol
					The symbol to be used as delimiter; if set to '\0', any space is delimiter (/[\s]/)
	@var		IniFormat::semicolon
					The rule of the semicolon character (use enum ::IniComments for this)
	@var		IniFormat::hash
					The rule of the hash character (use enum ::IniComments for this)
	@var		IniFormat::multiline_entries
					The multiline status of the document (use enum ::IniMultiline for this)
	@var		IniFormat::no_single_quotes
					If set to 1, makes the single-quote character (') a normal character
	@var		IniFormat::no_double_quotes
					If set to 1, makes the double-quote character (") a normal character
	@var		IniFormat::case_sensitive
					If set to 1, key- and section- names will not be rendered in lower case
	@var		IniFormat::preserve_spaces_in_values
					If set to 1, sequences of more than one space in values (/\s{2,}/) will not be collapsed
	@var		IniFormat::implicit_is_special
					If set to 1, the dispatch of implicit keys (see @ref libconfini) will always
					assign to IniDispatch::value and to IniDispatch::v_length the global variables
					::INI_IMPLICIT_VALUE and ::INI_IMPLICIT_V_LENGTH respectively; if set to 0,
					implicit keys will be considered as empty keys
	@var		IniFormat::disabled_can_be_implicit
					If set to 1, comments non containing a delimiter symbol will not be parsed as
					(disabled) implicit keys
	@var		IniFormat::no_disabled_after_spaces
					If set to 1, prevents that /[#;]\s+/[^\s][^\n]+/ be parsed as a disabled entry
	@var		IniFormat::_LIBCONFINI_RESERVED_
					Unused bits -- reserved for future extensions


	@struct		IniStatistics

	@var		IniStatistics::format
					The format of the INI file (see struct ::IniFormat)
	@var		IniStatistics::bytes
					The size of the parsed file in bytes
	@var		IniStatistics::member
					The size of the parsed file in members (nodes)


	@struct		IniDispatch

	@var		IniDispatch::append_to
					The current path
	@var		IniDispatch::format
					The format of the INI file (see struct ::IniFormat)
	@var		IniDispatch::type
					The dispatch type (see enum ::IniNodeType)
	@var		IniDispatch::data
					It can be a comment, a section or a key
	@var		IniDispatch::value
					It can be a key value or an empty string
	@var		IniDispatch::d_length
					The length of the string IniDispatch::data
	@var		IniDispatch::v_length
					The length of the string IniDispatch::value
	@var		IniDispatch::dispatch_id
					The dispatch id

**/



		/*\
		|*|
		|*|   PRIVATE ENVIRONMENT
		|*|
		\*/


/* Aliases just for code clarity... */

#define _LIBCONFINI_BOOL_ unsigned char
#define _LIBCONFINI_BYTE_ unsigned char
#define _LIBCONFINI_UINT_ unsigned long int
#define _LIBCONFINI_INT_ signed long int

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

	There can be infinite pairs here, but be aware of the lazy behavior of ini_getlazybool().
	Everything lowercase in this list!

**/
static const char *_LIBCONFINI_BOOLEANS_[][2] = {
	{ "no", "yes" },
	{ "false", "true" },
	{ "0", "1" }
};

/*
	This can be any character, in theory. But after the left-trim of each line a leading space
	works pretty well as metacharacter.
*/
#define _LIBCONFINI_INLINE_MARKER_ '\t'



/* ABSTRACT UTILITIES */

static const _LIBCONFINI_BYTE_ _SPACES_[] = { _LIBCONFINI_SIMPLE_SPACE_, '\t', '\v', '\f', _LIBCONFINI_LF_, _LIBCONFINI_CR_ };

/* _SPACES_ possible lengths */
#define _JUST_S_T_ 2
#define _NO_EOL_ 4
#define _WITH_EOL_ 6

static inline _LIBCONFINI_BOOL_ is_some_space (const char ch, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_BYTE_ sp_idx;
	for (sp_idx = 0; sp_idx < depth && ch != _SPACES_[sp_idx]; sp_idx++);
	return sp_idx < depth;
}

/**

	@brief			Soft left trim -- does not change the buffer
	@param			string			The target string
	@param			start_from		The offset where to start the left trim
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The offset of the first non-space character

**/
static inline _LIBCONFINI_UINT_ ltrim_s (const char *lt_s, const _LIBCONFINI_UINT_ start_from, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_UINT_ lt_i = start_from;
	for (; lt_s[lt_i] && is_some_space(lt_s[lt_i], depth); lt_i++);
	return lt_i;
}

/**

	@brief			Hard left trim -- **does** change the buffer
	@param			string			The target string
	@param			start_from		The offset where to start the left trim
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The offset of the first non-space character

**/
static inline _LIBCONFINI_UINT_ ltrim_h (char *lt_s, const _LIBCONFINI_UINT_ start_from, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_UINT_ lt_i = start_from;
	for (; lt_s[lt_i] && is_some_space(lt_s[lt_i], depth); lt_s[lt_i++] = '\0');
	return lt_i;
}


/**

	@brief			Unescaped hard left trim (left trim of /^(\s*|\\[\n\r])+/) -- **does** change the buffer
	@param			string			The target string
	@param			start_from		The offset where to start the left trim
	@return			The offset of the first non matching character

**/
static inline _LIBCONFINI_UINT_ ultrim_h (char *lt_s, const _LIBCONFINI_UINT_ start_from) {

	_LIBCONFINI_BYTE_ abacus = 12;
	_LIBCONFINI_UINT_ lt_i = start_from;

	/*

	Mask "abacus":

	FLAG_1	this is any space
	FLAG_2	this is not a backslash
	FLAG_4	previous was not a backslash
	FLAG_8	continue the loop

	*/

	for (; lt_s[lt_i] && (abacus & 8); lt_i++) {

		abacus = (abacus & 4) && is_some_space(lt_s[lt_i], _NO_EOL_) ? 13
			: lt_s[lt_i] == _LIBCONFINI_LF_ || lt_s[lt_i] == _LIBCONFINI_CR_ ? abacus | 5
			: (abacus & 4) && lt_s[lt_i] == _LIBCONFINI_BACKSLASH_ ? 10
			: abacus & 4;

		if (abacus & 1) {
			lt_s[lt_i] = '\0';
		}

		if ((abacus & 2) && (abacus & 4)) {
			lt_s[lt_i - 1] = '\0';
		}

	}

	return abacus & 8 ? lt_i : abacus & 4 ? lt_i - 1 : lt_i - 2;

}

/**

	@brief			Soft right trim -- does not change the buffer
	@param			string			The target string
	@param			length			The length of the string
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The length of the string until the last non-space character

**/
static inline _LIBCONFINI_UINT_ rtrim_s (const char *rt_s, const _LIBCONFINI_UINT_ length, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_UINT_ rt_i = length;
	for (; rt_i > 0 && is_some_space(rt_s[rt_i - 1], depth); rt_i--);
	return rt_i;
}

/**

	@brief			Hard right trim -- **does** change the buffer
	@param			string			The target string
	@param			length			The length of the string
	@param			depth			What is actually considered a space -- a number ranging from 1 to 6
	@return			The length of the string until the last non-space character

**/
static inline _LIBCONFINI_UINT_ rtrim_h (char *rt_s, const _LIBCONFINI_UINT_ length, const _LIBCONFINI_BYTE_ depth) {
	_LIBCONFINI_UINT_ rt_i = length;
	for (; rt_i > 0 && is_some_space(rt_s[rt_i - 1], depth); rt_s[--rt_i] = '\0');
	return rt_i;
}


/**

	@brief			Converts a string to lower case
	@param			string		The target string
	@return			Nothing

**/
static inline void string_tolower (char *str) {
	for (char *ch_ptr = str; *ch_ptr; ++ch_ptr) {
		*ch_ptr = *ch_ptr > 0x40 && *ch_ptr < 0x5b ? *ch_ptr | 0x60 : *ch_ptr;
	}
}



/* CONCRETE UTILITIES */

static inline _LIBCONFINI_BOOL_ is_parsable_char (const char ch, const IniFormat settings) {
	return (settings.semicolon == INI_PARSE_COMMENT && ch == _LIBCONFINI_SEMICOLON_) || (settings.hash == INI_PARSE_COMMENT && ch == _LIBCONFINI_HASH_);
}

static inline _LIBCONFINI_BOOL_ is_comm_char (const char ch, const IniFormat settings) {
	return (settings.semicolon != INI_NORMAL_CHARACTER && ch == _LIBCONFINI_SEMICOLON_) || (settings.hash != INI_NORMAL_CHARACTER && ch == _LIBCONFINI_HASH_);
}

static inline _LIBCONFINI_BOOL_ is_erase_char (const char ch, const IniFormat settings) {
	return (settings.semicolon == INI_ERASE_COMMENT && ch == _LIBCONFINI_SEMICOLON_) || (settings.hash == INI_ERASE_COMMENT && ch == _LIBCONFINI_HASH_);
}

/**

	@brief			Gets the position of the first delimiter out of quotes
	@param			string		The string to analyse
	@param			length		The length of the string
	@param			format		The format of the INI file
	@return			The offset of the delimiter or `len` if not found

**/
static inline _LIBCONFINI_UINT_ get_delimiter_pos (const char *str, const _LIBCONFINI_UINT_ len, const IniFormat settings) {

	_LIBCONFINI_UINT_ idx;
	_LIBCONFINI_BYTE_ qmask = 1;

	for (

		idx = 0;

			idx < len
			&&
			!(
				!(qmask & 6)
				&&
				(
					settings.delimiter_symbol ?
						str[idx] == settings.delimiter_symbol
						: is_some_space(str[idx], _NO_EOL_)
				)
			);

		qmask = (qmask & 1) && !settings.no_double_quotes && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? qmask ^ 4
				: (qmask & 1) && !settings.no_single_quotes && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? qmask ^ 2
				: str[idx] == _LIBCONFINI_BACKSLASH_ ? qmask ^ 1
				: qmask | 1,
		idx++

	);

	return idx;

}


/**

	@brief			Replaces /\\(\n\r?|\r\n?)s*[#;]/ or /\\(\n\r?|\r\n?)/ with "$1".
	@param			string		Target string
	@param			length		Length of the string
	@param			is_disabled	The string represents an disabled item
	@return			Number of characters removed

**/
static _LIBCONFINI_UINT_ unescape_cr_lf (char * nstr, const _LIBCONFINI_UINT_ len, const _LIBCONFINI_BOOL_ is_disabled, const IniFormat settings) {

	_LIBCONFINI_UINT_ idx, iter, lshift;
	_LIBCONFINI_BOOL_ is_escaped = _LIBCONFINI_FALSE_;
	_LIBCONFINI_BYTE_ cr_or_lf = 5;

	for (is_escaped = _LIBCONFINI_FALSE_, idx = 0, lshift = 0; idx < len; idx++) {

		if (is_escaped && (nstr[idx] == _SPACES_[cr_or_lf] || nstr[idx] == _SPACES_[cr_or_lf ^= 1])) {

			for (

				iter = idx,
				idx += nstr[idx + 1] == _SPACES_[cr_or_lf ^ 1];

					iter < idx + 1;

				nstr[iter - lshift - 1] = nstr[iter],
				iter++

			);

			lshift++;

			if (is_disabled) {

				iter = ltrim_s(nstr, iter, _NO_EOL_);

				if (is_parsable_char(nstr[iter], settings)) {

					lshift += iter - idx;
					idx = iter;

				}

			}

			is_escaped = 0;

		} else {

			is_escaped = nstr[idx] == _LIBCONFINI_BACKSLASH_ ? !is_escaped : 0;

			if (lshift) {

				nstr[idx - lshift] = nstr[idx];

			}

		} 

	}

	for (idx = len - lshift; idx < len; nstr[idx++] = '\0');

	return len - lshift;

}


/**

	@brief			Out of quotes similar to ECMAScript string.replace(/^[\n\r]\s*|(\s)+/g, "$1")
	@param			string		The string to collapse
	@param			format		The format of the INI file
	@return			The new length of the string

**/
static _LIBCONFINI_UINT_ collapse_spaces (char *str, const IniFormat settings) {

	/*

	Mask "abacus":

	FLAG_1	backslashes immediately preceding this character are even
	FLAG_2	unescaped single quotes are odd until now
	FLAG_4	unescaped double quotes are odd until now
	FLAG_8	the string does not start with a space

	*/

	_LIBCONFINI_UINT_ idx, lshift;
	_LIBCONFINI_BYTE_ abacus = 1 | (is_some_space(*str, _WITH_EOL_) << 3);

	for (lshift = 0, idx = 0; str[idx]; idx++) {

		if (!(abacus & 6) && is_some_space(str[idx], _WITH_EOL_)) {

			if (abacus & 8) {

				lshift++;

			} else {

				str[idx - lshift] = _LIBCONFINI_SIMPLE_SPACE_;
				abacus |= 8;

			}

				abacus |= 1;

		} else {

			abacus = (abacus & 1) && !settings.no_double_quotes && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 4
					: (abacus & 1) && !settings.no_single_quotes && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 2
					: str[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 1
					: abacus | 1;

			if (lshift) {

					str[idx - lshift] = str[idx];

			}

			abacus &= 7;

		}

	}

	for (idx -= (abacus & 8) ? ++lshift : lshift; str[idx]; str[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Sanitizes the name of a section
	@param			section_name		The target string
	@param			format				The format of the INI file
	@return			The new length of the string

	Out of quotes, similar to ECMAScript `section_name.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/(\s)+/g, " ")`

	A section name can start with a dot (append), but cannot end with a dot. Spaces sorrounding dots
	will be removed. Single quotes or double quotes (if active and present) prevent changes.

**/
static _LIBCONFINI_UINT_ sanitize_section_name (char *str, const IniFormat settings) {

	_LIBCONFINI_UINT_ idx, lshift;
	unsigned int abacus;

	/*

	Mask "abacus":

	FLAG_1			backslashes immediately preceding this character are even
	FLAG_2			unescaped single quotes are odd until now
	FLAG_4			unescaped double quotes are odd until now
	FLAG_8			the string does not start with a new line
	FLAG_16			more than one space is here
	FLAG_32			at least one dot is here
	FLAG_64			one or more spaces follow a dot
	FLAG_128		more than one dot is here
	FLAG_256		continue the shift

	*/

	for (abacus = is_some_space(*str, _WITH_EOL_) ? 65 : 9, lshift = 0, idx = 0; str[idx]; idx++) {

		if (!(abacus & 6) && is_some_space(str[idx], _WITH_EOL_)) {

			if (abacus & 80) {

				lshift++;

			}

			if (!(abacus & 16)) {

				str[idx - lshift] = abacus & 32 ? _LIBCONFINI_SUBSECTION_ : _LIBCONFINI_SIMPLE_SPACE_;
				abacus &= 9;
				abacus |= 16;

			}

			abacus |= 1;

		} else if (!(abacus & 6) && str[idx] == _LIBCONFINI_SUBSECTION_) {

			if ((abacus & 144) && (abacus & 8)) {

				lshift++;

			}

			abacus |= abacus & 144 ? 361 : 489;

		} else {

				abacus &= 7;
				abacus = (abacus & 1) && !settings.no_double_quotes && str[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 4
					: (abacus & 1) && !settings.no_single_quotes && str[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 2
					: str[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 1
					: abacus | 1;
				abacus |= 264;

		}

		if (abacus & 256) {

			str[idx - lshift] = str[idx];
			abacus &= 255;

		}

	}

	for (idx -= abacus & 240 ? ++lshift : lshift; str[idx]; str[idx++] = '\0');

	return idx - lshift;

}


/**

	@brief			Removes all comment initializers ('#' or ';') from the beginning of each line of a comment
	@param			commstr		The comment to sanitize
	@param			commstr		The length of commstr
	@param			format		The format of the INI file
	@return			The new length of the string

**/
static _LIBCONFINI_UINT_ uncomment (char *commstr, _LIBCONFINI_UINT_ len, const IniFormat format) {

	_LIBCONFINI_UINT_ lshift, idx;

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

		Mask "abacus":

		FLAG_1		Don't erase any character
		FLAG_2		Backslashes immediately preceding this character are odd
		FLAG_4		This character is a backslash
		FLAG_8		This character is a comment character and follows /(\n\s*|\r\s*)/
		FLAG_16		This character is a part of a group of spaces following a new line (/(\n|\r)[\t \v\f]+/)
		FLAG_32		This character is not a new line character (/[^\r\n]/)

		*/

		for (abacus = 3, lshift = 0, idx = 0; idx < len; idx++) {

			abacus		=	commstr[idx] == _LIBCONFINI_BACKSLASH_ ?
								((abacus & 35) | 32) ^ 2
							: commstr[idx] == _LIBCONFINI_LF_ || commstr[idx] == _LIBCONFINI_CR_ ?
								(abacus & 16) | ((abacus << 1) & 4)
							: !(abacus & 32) && is_comm_char(commstr[idx], format) ?
								(abacus & 40) | 8
							: !(abacus & 40) && is_some_space(commstr[idx], _NO_EOL_)?
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
static _LIBCONFINI_BYTE_ get_type_as_active (const char *nodestr, const _LIBCONFINI_UINT_ len, const _LIBCONFINI_BOOL_ wanna_be_active, const IniFormat settings) {

	if (!len || *nodestr == settings.delimiter_symbol || is_comm_char(*nodestr, settings)) {

		return INI_UNKNOWN;

	}

	_LIBCONFINI_BYTE_ qmask;
	_LIBCONFINI_UINT_ idx;

	if (*nodestr == _LIBCONFINI_OPEN_SECTION_) {

		for (qmask = 1, idx = 1; idx < len; idx++) {

			if (nodestr[idx] == _LIBCONFINI_CLOSE_SECTION_ && !(qmask & 6)) {

				return INI_SECTION;

			}

			qmask = (qmask & 1) && !settings.no_double_quotes && nodestr[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? qmask ^ 4
					: (qmask & 1) && !settings.no_single_quotes && nodestr[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? qmask ^ 2
					: nodestr[idx] == _LIBCONFINI_BACKSLASH_ ? qmask ^ 1
					: qmask | 1;

		}

		return INI_UNKNOWN;

	}

	/* It can be just a key... */

	idx = get_delimiter_pos(nodestr, len, settings);

	if (wanna_be_active && (!settings.disabled_can_be_implicit && idx == len)) {

		return INI_UNKNOWN;

	}

	return INI_KEY;

}


/**

	@brief			Recursive function, examines a (single-/multi- line) segment and searches for sub-segments.
	@param			segment	Segment to examine
	@param			format	The format of the INI file
	@return			Number of sub-segments found, included itself

**/
static _LIBCONFINI_UINT_ further_cuts (char *segment, const IniFormat settings) {

	if (!*segment) { return 0; }

	_LIBCONFINI_BOOL_ show_comments = !is_erase_char(*segment, settings);
	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_UINT_ idx, focus_at, search_at = 0, unparse_at = 0, fragm_size = show_comments;

	if (

			(!settings.multiline_entries && is_comm_char(*segment, settings))
			||
			(
				is_parsable_char(*segment, settings)
				&&
				!(settings.no_disabled_after_spaces && is_some_space(segment[1], _NO_EOL_))
			)

		) {

		/*

		Mask "abacus":

		FLAG_1	backslashes immediately preceding this character are even
		FLAG_2	unescaped single quotes are even until now
		FLAG_4	unescaped double quotes are even until now
		FLAG_8	this does not immediately follow [\n\r]\s*
		FLAG_16	the previous character was a space

		*/

		for (abacus = 15, idx = ltrim_s(segment, 1, _NO_EOL_); segment[idx]; idx++) {

				if (is_comm_char(segment[idx], settings)) {

					/* Search for inline comments in (supposedly) disabled items */

					if (abacus == 31) {

						focus_at = ltrim_s(segment, 1, _NO_EOL_);

						if (get_type_as_active(segment + focus_at, idx - focus_at, _LIBCONFINI_TRUE_, settings)) {

							segment[idx] = _LIBCONFINI_INLINE_MARKER_;
							segment[idx - 1] = '\0';
							rtrim_h(segment, idx - 1, _NO_EOL_);

							if (show_comments) {

								fragm_size++;

							}

							if (settings.multiline_entries) {

								unparse_at = idx + 1;
								break;

							}

						} else if (settings.multiline_entries) {

							unparse_at = 1;
							break;

						}

					}

					abacus &= 7;
					abacus |= 9;

				} else if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

					/* Search for /\\(?:\n\r?|\r\n?)\s*[^;#]/ in multiline disabled items */

					idx = ltrim_s(segment, idx + 1, _WITH_EOL_);

					if (
							(settings.no_disabled_after_spaces && is_some_space(segment[idx + 1], _NO_EOL_))
							||
							!(
								(is_comm_char(segment[idx], settings) && !settings.multiline_entries)
								||
								(is_parsable_char(segment[idx], settings) && settings.multiline_entries < 2)
							)
						) {

						rtrim_h(segment, idx, _WITH_EOL_);

						if (segment[idx]) {

							search_at = idx + 1;

							if (show_comments) {

								fragm_size++;

							}

						}

						break;

					}

					abacus &= 7;
					abacus |= 1;

				} else if (is_some_space(segment[idx], _NO_EOL_)) {

					abacus &= 7;
					abacus |= 25;

				} else {

					abacus &= 15;
					abacus = (abacus & 1) && !settings.no_double_quotes && segment[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 4
							: (abacus & 1) && !settings.no_single_quotes && segment[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 2
							: segment[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 1
							: abacus | 1;
					abacus |= 8;

				}

		}

		if (settings.multiline_entries && !unparse_at) {

			focus_at = ltrim_s(segment, 1, _NO_EOL_);

			if (segment[focus_at] && !get_type_as_active(segment + focus_at, idx - focus_at, _LIBCONFINI_TRUE_, settings)) {

				unparse_at = 1;
			}

		}

	} else if (is_comm_char(*segment, settings)) {

		unparse_at = 1;

	} else {

		search_at = 1;

	}

	if (unparse_at) {

		/* Cut unparsable multiline comments */

		cut_unparsable:

		for (idx = unparse_at; segment[idx]; idx++) {

			if (segment[idx] == _LIBCONFINI_LF_ || segment[idx] == _LIBCONFINI_CR_) {

				fragm_size += further_cuts(segment + ultrim_h(segment, idx), settings);
				break;

			}

		}

	}

	if (search_at) {

		/* Search for inline comments in active items */

		for (abacus = 1, idx = search_at; segment[idx]; idx++) {

			if (is_comm_char(segment[idx], settings) && is_some_space(segment[idx - 1], _WITH_EOL_) && !(abacus & 6)) {

				segment[idx] = _LIBCONFINI_INLINE_MARKER_;
				segment[idx - 1] = '\0';
				rtrim_h(segment, idx - 1, _NO_EOL_);
				unparse_at = idx + 1;
				search_at = 0;

				if (show_comments) {

					fragm_size++;

				}

				goto cut_unparsable;

			} else {

				abacus = (abacus & 1) && !settings.no_double_quotes && segment[idx] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 4
						: (abacus & 1) && !settings.no_single_quotes && segment[idx] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 2
						: segment[idx] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 1
						: abacus | 1;

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
	@param			path				The path of the INI file
	@param			format				The format of the INI file
	@param			f_init				The function that will be invoked before the dispatch, or NULL
	@param			f_foreach			The function that will be invoked for each dispatch, or NULL
	@param			other_argument		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

**/
unsigned int load_ini_file (
	const char *path,
	const IniFormat format,
	int (*f_init)(
		IniStatistics *statistics,
		void *init_other
	),
	int (*f_foreach)(
		IniDispatch *dispatch,
		void *foreach_other
	),
	void *other_argument
) {

	FILE *config_file = fopen(path, "r");

	if (config_file == NULL) {

		return CONFINI_ENOENT;

	}

	unsigned int return_value = 0;
	char *cache;
	_LIBCONFINI_UINT_ len;

	fseek(config_file, 0, SEEK_END);
	len = ftell(config_file);
	rewind(config_file);
	cache = (char *) malloc((len + 1) * sizeof(char));

	if (cache == NULL) {

		return CONFINI_ENOMEM;

	}

	if (fread(cache, sizeof(char), len, config_file) < len) {

		return_value = CONFINI_EIO;
		goto free_and_exit;

	}

	fclose(config_file);
	cache[len] = '\0';

	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_UINT_ idx, node_at, tmp_int;

	IniStatistics this_doc = {
			.format = format,
			.bytes = len,
			.members = 0
	};

	/* Examine and isolate each segment */

	for (tmp_int = 5, abacus = 0, node_at = 0, idx = 0; idx < len; idx++) {

		if (cache[idx] == _SPACES_[tmp_int] || cache[idx] == _SPACES_[tmp_int ^= 1]) {

			if (format.multiline_entries < 3 && abacus) {

				idx += cache[idx + 1] == _SPACES_[tmp_int ^ 1];

			} else {

				cache[idx] = '\0';
				this_doc.members += further_cuts(cache + ultrim_h(cache, node_at), format);
				node_at = idx + 1;

			}

			abacus = 0;

		} else if (cache[idx] == _LIBCONFINI_BACKSLASH_) {

			abacus ^= 1;

		} else {

			if (!cache[idx]) {

				cache[idx] = _LIBCONFINI_SIMPLE_SPACE_;

			}

			abacus = 0;

		}

	}

	this_doc.members += further_cuts(cache + ultrim_h(cache, node_at), format);

	/* Debug */
	/*

	for (_LIBCONFINI_UINT_ tmp = 0; tmp < len; tmp++) {
		putchar(cache[tmp] > 0 ? cache[tmp] : '$');
	}
	putchar(_LIBCONFINI_LF_);

	*/

	if (f_init && f_init(&this_doc, other_argument)) {

		return_value = CONFINI_EIINTR;
		goto free_and_exit;

	}

	if (!f_foreach) {

		goto free_and_exit;

	}

	/* Dispatch the parsed input */

	_LIBCONFINI_BOOL_ parent_is_disabled = _LIBCONFINI_FALSE_;
	_LIBCONFINI_UINT_ parse_at, real_parent_len = 0, parent_len = 0;
	char *parent = cache + len, *subparent = parent, *real_parent = parent;
	IniDispatch this_d = {
		.format = format,
		.dispatch_id = 0
	};

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
		this_d.d_length = idx - node_at;
		this_d.v_length = 0;

		if (is_parsable_char(*this_d.data, format)) {

			parse_at = ltrim_s(cache, node_at + 1, _NO_EOL_);

			/*

			Mask "abacus":

					FLAG_1			continue the loop
					FLAG_2			we are not in \n[ \t\v\f]*
					FLAG_4			this is a space and FLAG_2 == TRUE

			*/

			for (abacus = 1, tmp_int = 1; abacus && tmp_int < this_d.d_length; tmp_int++) {

				abacus		=	this_d.data[tmp_int] == _LIBCONFINI_LF_ || this_d.data[tmp_int] == _LIBCONFINI_CR_ ? 1
								: (abacus & 4) && is_comm_char(this_d.data[tmp_int], format) ? 0
								: (abacus & 2) && is_some_space(this_d.data[tmp_int], _NO_EOL_) ? 7
								: 3;

			}

			this_d.type = tmp_int == this_d.d_length ? get_type_as_active(cache + parse_at, idx - parse_at, _LIBCONFINI_TRUE_, format) : 0;

			if (this_d.type) {

				this_d.data = cache + parse_at;
				this_d.d_length = idx - parse_at;

			}

			this_d.type |= 4;

		} else if (is_comm_char(*this_d.data, format)) {

			this_d.type = INI_COMMENT;

		} else if (*this_d.data == _LIBCONFINI_INLINE_MARKER_) {

			this_d.type = INI_INLINE_COMMENT;

		} else {

			this_d.type = get_type_as_active(this_d.data, this_d.d_length, _LIBCONFINI_FALSE_, format);

		}

		if (parent_len && *subparent) {

			tmp_int = 0;

			do {

				parent[tmp_int + parent_len] = subparent[tmp_int];

			} while (subparent[tmp_int++]);

			parent_len += tmp_int - 1;
			subparent = parent + parent_len;

		}

		if (parent_is_disabled && !(this_d.type & 4)) {

			real_parent[real_parent_len] = '\0';
			parent_len = real_parent_len;
			parent = real_parent;
			parent_is_disabled = _LIBCONFINI_FALSE_;

		} else if (!parent_is_disabled && this_d.type == INI_DISABLED_SECTION) {

			real_parent_len = parent_len;
			real_parent = parent;
			parent_is_disabled = _LIBCONFINI_TRUE_;

		}

		this_d.append_to = parent;

		this_d.d_length		=	this_d.type == INI_COMMENT || this_d.type == INI_INLINE_COMMENT ? 
									uncomment(this_d.data, idx - node_at, format)
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

					Mask "abacus":

					FLAG_1			backslashes immediately preceding this character are even
					FLAG_2			unescaped single quotes are odd until now
					FLAG_4			unescaped double quotes are odd until now

				*/

				for (

					abacus = 1, tmp_int = 0;

						this_d.data[tmp_int]
						&&
						((abacus & 6) || this_d.data[tmp_int] != _LIBCONFINI_CLOSE_SECTION_);

					abacus = (abacus & 1) && !format.no_double_quotes && this_d.data[tmp_int] == _LIBCONFINI_DOUBLE_QUOTES_ ? abacus ^ 4
							: (abacus & 1) && !format.no_single_quotes && this_d.data[tmp_int] == _LIBCONFINI_SINGLE_QUOTES_ ? abacus ^ 2
							: this_d.data[tmp_int] == _LIBCONFINI_BACKSLASH_ ? abacus ^ 1
							: abacus | 1,
					tmp_int++

				);

				while (this_d.data[tmp_int]) {

					this_d.data[tmp_int++] = '\0';

				}

				this_d.d_length = sanitize_section_name(this_d.data, format);

				if (parent_len && *this_d.data == _LIBCONFINI_SUBSECTION_) {

					subparent = this_d.data;

				} else {

					parent = this_d.data + (!parent_len && *this_d.data == _LIBCONFINI_SUBSECTION_);
					parent_len = this_d.d_length - (!parent_len && *this_d.data == _LIBCONFINI_SUBSECTION_);
					subparent = cache + idx;
					this_d.append_to = subparent;

				}

				if (!format.case_sensitive) {

					string_tolower(this_d.data);

				}

				break;

			case INI_KEY:
			case INI_DISABLED_KEY:

				tmp_int = get_delimiter_pos(this_d.data, this_d.d_length, format);

				if (this_d.d_length && tmp_int && tmp_int < this_d.d_length) {

					this_d.data[tmp_int] = '\0';

					if (format.preserve_spaces_in_values) {

						this_d.v_length = this_d.d_length - ltrim_h(this_d.data, tmp_int + 1, _NO_EOL_);
						this_d.value = this_d.data + this_d.d_length - this_d.v_length;

					} else {

						this_d.value = this_d.data + tmp_int + 1;
						this_d.v_length = collapse_spaces(this_d.value, format);

					}

					this_d.d_length = collapse_spaces(this_d.data, format);

				} else if (format.implicit_is_special) {

					this_d.value = INI_IMPLICIT_VALUE;
					this_d.v_length = INI_IMPLICIT_V_LENGTH;
				}

				if (!format.case_sensitive) {

					string_tolower(this_d.data);

				}

				break;

			case INI_COMMENT:
			case INI_INLINE_COMMENT:

				this_d.append_to = cache + idx;

		}

		if (f_foreach(&this_d, other_argument)) {

			return_value = CONFINI_EFEINTR;
			goto free_and_exit;

		}

		this_d.dispatch_id++;
		node_at = idx + 1;

	}

	free_and_exit:

	free(cache);
	return return_value;

}



/* UTILITIES JUST FOR THE USER (NOT USED HERE) */


/**

	@brief			Converts an IniFormat into an ::ini_format_uint
	@param			source		The IniFormat to be read
	@return			The mask representing the format

**/
ini_format_uint get_ini_format_mask (const IniFormat source) {

	ini_format_uint mask = 0;

	unsigned int bitpos = 0;

	#define _LIBCONFINI_ASSIGN_TO_MASK_(SIZE, PROPERTY, IGNORE_ME) \
		mask |= source.PROPERTY << bitpos;\
		bitpos += SIZE;

	_LIBCONFINI_EXPAND_SETTINGS_MACRO_AS_(_LIBCONFINI_ASSIGN_TO_MASK_)

	#undef _LIBCONFINI_ASSIGN_TO_MASK_

	return mask;

}


/**

	@brief			Sets all the values of an IniFormat by reading them from an ::ini_format_uint
	@param			mask		The ::ini_format_uint to be read
	@param			dest		The IniFormat to be set
	@return			Nothing

**/
void read_ini_format_mask (ini_format_uint mask, IniFormat *dest) {

	#define _LIBCONFINI_ASSIGN_TO_INI_TYPE_(SIZE, PROPERTY, IGNORE_ME) \
		dest->PROPERTY = mask;\
		mask >>= SIZE;

	_LIBCONFINI_EXPAND_SETTINGS_MACRO_AS_(_LIBCONFINI_ASSIGN_TO_INI_TYPE_)

	#undef _LIBCONFINI_ASSIGN_TO_INI_TYPE_

}


/**

	@brief			Unescapes a string and removes all its unescaped quotes (if single/double quotes are active)
	@param			ini_string	The string to be unescaped
	@param			format		The format of the INI file
	@return			The new length of the string

**/
unsigned long int ini_unquote (char *ini_string, const IniFormat format) {

	_LIBCONFINI_BYTE_ qmask;
	_LIBCONFINI_UINT_ lshift, nbacksl, idx;

	/*

	Mask "qmask":

	FLAG_1		unescaped single quotes are even until now
	FLAG_2		unescaped double quotes are even until now
	FLAG_4		this is an unescaped single quote and single quotes are metacharacters
	FLAG_8		this is an unescaped double quote and double quotes are metacharacters

	*/

	for (qmask = 3, lshift = 0, nbacksl = 0, idx = 0; ini_string[idx]; idx++) {

		qmask = (qmask & 3)
				| (
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
unsigned long int ini_array_getlength (const char *ini_string, const char delimiter, const IniFormat format) {

	unsigned long int arr_length = 0;
	_LIBCONFINI_UINT_ idx;
	_LIBCONFINI_BYTE_ abacus;

	/*

	Mask "abacus":

	FLAG_1	backslashes immediately preceding this character are even
	FLAG_2	unescaped single quotes are odd until now
	FLAG_4	unescaped double quotes are odd until now
	FLAG_8	this is not a delimiter
	FLAG_16	continue the loop

	*/

	for (abacus = 17, idx = 0; abacus; idx++) {

		abacus =	ini_string[idx] == delimiter ? abacus & 23
					: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ? (abacus | 8) ^ 1
					: ini_string[idx] ? (
						(abacus | 9)
						^ (
							!format.no_double_quotes && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ && (abacus & 1) ? 4
							: !format.no_single_quotes && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ && (abacus & 1) ? 2
							: 0
						)
					)
					: 0;

		if (!(abacus & 14)) {

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
	@param			other_argument		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

**/
unsigned int ini_array_foreach (
	const char *ini_string,
	const char delimiter,
	const IniFormat format,
	int (*f_foreach)(
		const char *member,
		unsigned int offset,
		unsigned int length,
		void *foreach_other
	),
	void *other_argument
) {

	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_UINT_ idx, offs;

	/* Mask "abacus": as above */

	for (abacus = 17, offs = 0, idx = 0; abacus; idx++) {

		abacus = ini_string[idx] == delimiter ? abacus & 23
				: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ? (abacus | 8) ^ 1
				: ini_string[idx] ? (
					(abacus | 9)
					^ (
						!format.no_double_quotes && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ && (abacus & 1) ? 4
						: !format.no_single_quotes && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ && (abacus & 1) ? 2
						: 0
					)
				)
				: 0;

		if (!(abacus & 14)) {

			offs = ltrim_s(ini_string, offs, _WITH_EOL_);

			if (f_foreach(ini_string, offs, rtrim_s(ini_string + offs, idx - offs, _WITH_EOL_), other_argument)) {

				return CONFINI_EFEINTR;

			}

			offs = idx + 1;

		}

	}

	return 0;

}


/**

	@brief			Splits an INI array and calls a custom function for each member.
	@param			ini_string			The stringified array
	@param			delimiter			The delimiter of the array members
	@param			format				The format of the INI file
	@param			f_foreach			The function that will be invoked for each array member
	@param			other_argument		A custom argument, or NULL
	@return			Zero for success, otherwise an error code

**/
unsigned int ini_split_array (
	char *ini_string,
	const char delimiter,
	const IniFormat format,
	int (*f_foreach)(
		char *element,
		unsigned int length,
		void *foreach_other
	),
	void *other_argument
) {

	_LIBCONFINI_BYTE_ abacus;
	_LIBCONFINI_UINT_ offs, idx;

	/* Mask "abacus": as above */

	for (offs = 0, abacus = 17, idx = 0; abacus; idx++) {

		abacus		=	ini_string[idx] == delimiter ?
							abacus & 23
						: ini_string[idx] == _LIBCONFINI_BACKSLASH_ ?
							(abacus | 8) ^ 1
						: ini_string[idx] ?
							(
								(abacus | 9)
								^ (
									!format.no_double_quotes && ini_string[idx] == _LIBCONFINI_DOUBLE_QUOTES_ && (abacus & 1) ? 4
									: !format.no_single_quotes && ini_string[idx] == _LIBCONFINI_SINGLE_QUOTES_ && (abacus & 1) ? 2
									: 0
								)
							)
						:
							0;

		if (!(abacus & 14)) {

			ini_string[idx] = '\0';
			offs = ltrim_h(ini_string, offs, _WITH_EOL_);

			if (f_foreach(ini_string + offs, rtrim_h(ini_string + offs, idx - offs, _WITH_EOL_), other_argument)) {

				return CONFINI_EFEINTR;

			}

			offs = idx + 1;

		}

	}

	return 0;

}


/**

	@brief			Checks if a string matches *exactly* one of the booleans listed in the private constant @link #_LIBCONFINI_BOOLEANS_
	@param			ini_string			A string to be checked
	@param			return_value		A value that will be returned if no matching boolean will be found
	@return			The matching boolean value (0 or 1) or **return_value** if no boolean will be found

**/
signed int ini_getbool (const char *ini_string, const signed int return_value) {

	unsigned short pair_idx, bool_idx, chr_idx;

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

	@brief			Checks if the first letter of a string matches the first letter of one of the booleans listed in _LIBCONFINI_BOOLEANS_
	@param			ini_string			A string to be checked
	@param			return_value		A value that will be returned if no matching boolean will be found
	@return			The matching boolean value (0 or 1) or **return_value** if no boolean will be found

**/
signed int ini_getlazybool (const char *ini_string, const signed int return_value) {

	unsigned short pair_idx, bool_idx;
	unsigned char ch = *ini_string > 0x40 && *ini_string < 0x5b ? *ini_string | 0x60 : *ini_string;

	for (pair_idx = 0; pair_idx < sizeof(_LIBCONFINI_BOOLEANS_) / 2 / sizeof(char *); pair_idx++) {

		for (bool_idx = 0; bool_idx < 2; bool_idx++) {

			if (ch == *_LIBCONFINI_BOOLEANS_[pair_idx][bool_idx]) {

				return bool_idx;

			}

		}

	}

	return return_value;

}

/* WRAPPERS -- In case you don't want to "#include <stdlib.h>" in your source */

/*
long double (*ini_getlfloat)(const char *ini_string) = &atolf;
*/
/* Where is atolf?? */

double (*ini_getfloat) (const char *ini_string) = &atof;

long long int (*ini_getllint) (const char *ini_string) = &atoll;

long int (*ini_getlint) (const char *ini_string) = &atol;

int (*ini_getint) (const char *ini_string) = &atoi;


/* EOF */

