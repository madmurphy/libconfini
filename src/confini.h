/**

	@file		confini.h
	@brief		libconfini header
	@author		Stefano Gioffr&eacute;
	@copyright 	GNU Public License v3
	@date		2016-2017

**/

#ifndef _LIBCONFINI_HEADER_
#define _LIBCONFINI_HEADER_

#include <stdio.h>



/* PRIVATE (HEADER-SCOPED) MACROS */


#define _LIBCONFINI_FORMAT_AS_DECLARATION_(SIZE, PROPERTY, VALUE) unsigned char PROPERTY:SIZE;
#define _LIBCONFINI_FORMAT_AS_ASSIGNEMENT_(SIZE, PROPERTY, VALUE) VALUE,
#define _LIBCONFINI_FORMAT_STRUCT_ struct IniFormat { _LIBCONFINI_EXPAND_MODEL_FORMAT_AS_(_LIBCONFINI_FORMAT_AS_DECLARATION_) }
#define _LIBCONFINI_DEFAULT_FORMAT_ { _LIBCONFINI_EXPAND_MODEL_FORMAT_AS_(_LIBCONFINI_FORMAT_AS_ASSIGNEMENT_) }



/* GLOBAL MACROS */


#define _LIBCONFINI_EXPAND_MODEL_FORMAT_AS_(______)                            /*-*\

    ______(     BITS      NAME                        DEFAULT VALUE            )/--/
                                                                               /-*/\
    ______(     8,        delimiter_symbol,           INI_EQUALS               )/*-/
                                                                               /-*/\
    ______(     2,        semicolon,                  INI_PARSE_COMMENT        )   \
    ______(     2,        hash,                       INI_PARSE_COMMENT        )   \
    ______(     2,        multiline_entries,          INI_EVERYTHING_MULTILINE )   \
    ______(     1,        case_sensitive,             0                        )   \
    ______(     1,        no_spaces_in_names,         0                        )/*-/
                                                                               /-*/\
    ______(     1,        no_single_quotes,           0                        )   \
    ______(     1,        no_double_quotes,           0                        )   \
    ______(     1,        implicit_is_not_empty,      0                        )   \
    ______(     1,        do_not_collapse_values,     0                        )   \
    ______(     1,        no_disabled_after_space,    0                        )   \
    ______(     1,        disabled_can_be_implicit,   0                        )   \
    ______(     2,        _LIBCONFINI_RESERVED_,      0                        )/*-/
                                                                               /-*/



/* PUBLIC TYPEDEFS */


/** @brief	24-bit bitfield representing a unique format of an INI file (INI dialect) -- `sizeof(IniFormat)` should be `3` **/
typedef _LIBCONFINI_FORMAT_STRUCT_ IniFormat;

/** @brief	Global statistics about an INI file **/
typedef struct IniStatistics {
	const IniFormat format;
	const size_t bytes;
	const size_t members;
} IniStatistics;

/** @brief	Dispatch of a single INI member **/
typedef struct IniDispatch {
	const IniFormat format;
	unsigned short int type;
	char *data;
	char *value;
	char *append_to;
	size_t d_len;
	size_t v_len;
	size_t at_len;
	size_t dispatch_id;
} IniDispatch;

/** @brief	24-bit bitmask representing the format of an INI file **/
typedef size_t IniFormatId;



/* PUBLIC FUNCTIONS */


extern int load_ini_file (
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
);

extern int load_ini_path (
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
);

extern void ini_dispatch_case_insensitive_lowercase (
	_Bool b_lowercase
);

extern void ini_set_implicit_value (
	char * const implicit_value,
	const size_t implicit_v_len
);

extern IniFormatId ini_format_get_id (
	const IniFormat format
);

extern void ini_format_set_to_id (
	IniFormat *dest_format,
	IniFormatId format_id
);

extern _Bool ini_string_match_ss (
	const char * const simple_string_a,
	const char * const simple_string_b,
	const IniFormat format
);

extern _Bool ini_string_match_si (
	const char * const simple_string,
	const char * const ini_string,
	const IniFormat format
);

extern _Bool ini_string_match_ii (
	const char * const ini_string_a,
	const char * const ini_string_b,
	const IniFormat format
);

extern size_t ini_unquote (
	char * const ini_string,
	const IniFormat format
);

extern size_t ini_array_get_length (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format
);

extern int ini_array_foreach (
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
);

extern size_t ini_collapse_array (
	char * const ini_string,
	const char delimiter,
	const IniFormat format
);

extern int ini_split_array (
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
);

extern signed int ini_get_bool (
	const char * const ini_string,
	const signed int return_value
);

extern signed int ini_get_lazy_bool (
	const char * const ini_string,
	const signed int return_value
);



/* PUBLIC LINKS */


extern int (* const ini_get_int) (
	const char *ini_string
);

extern long int (* const ini_get_lint) (
	const char *ini_string
);

extern long long int (* const ini_get_llint) (
	const char *ini_string
);

extern double (* const ini_get_float) (
	const char *ini_string
);



/* PUBLIC CONSTANTS AND VARIABLES */


/** @brief	Error flag (not present in user-generated interruptions) -- its value should be considered opaque **/
#define CONFINI_ERROR 4

/** @brief	Error codes -- the actual values of each constant should be considered opaque **/
enum ConfiniErrorNo {
	CONFINI_EIINTR = 1,		/**< Interrupted by the user during `f_init()` **/
	CONFINI_EFEINTR = 2,		/**< Interrupted by the user during `f_foreach()` **/
	CONFINI_ENOENT = 4,		/**< File inaccessible **/
	CONFINI_ENOMEM = 5,		/**< Error allocating memory **/
	CONFINI_EIO = 6,		/**< Error reading the file **/
	CONFINI_EFEOOR = 7		/**< The loop is longer than expected (out of range) **/
};

/** @brief	Most used delimiters (but a delimiter can also be any other ASCII character) **/
enum IniDelimiters {
	INI_ANY_SPACE = 0,		/**< In multiline INIs: `/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`, in non-multiline INIs: `/[\t \v\f])+/` **/
	INI_EQUALS = '=',		/**< `=` **/
	INI_COLON = ':'			/**< `:` **/
};

/** @brief	INI nodes types **/
enum IniNodeType {
	INI_UNKNOWN = 0,
	INI_VALUE = 1,			/**< Not used here, but available for user's implementations **/
	INI_SECTION = 2,
	INI_KEY = 3,
	INI_COMMENT = 4,
	INI_INLINE_COMMENT = 5,
	INI_DISABLED_SECTION = 6,
	INI_DISABLED_KEY = 7
};

/** @brief	Possible values for IniFormat::semicolon and IniFormat::hash (i.e., meaning of `/\s+[#;]/` in respect to a format) **/
enum IniComments {
	INI_PARSE_COMMENT = 0,		/**< This character opens a comment or a disabled entry **/
	INI_SHOW_COMMENT = 1,		/**< This character opens a comment **/
	INI_FORGET_COMMENT = 2,		/**< This character opens a comment that must be ignored **/
	INI_NORMAL_CHARACTER = 3	/**< This is a normal character **/
};

/** @brief	Possible values of IniFormat::multiline_entries **/
enum IniMultiline {
	INI_EVERYTHING_MULTILINE = 0,		/**< Comments, section paths and keys -- disabled or not -- are allowed to be multiline. **/
	INI_ACTIVE_AND_DISABLED_MULTILINE = 1,	/**< Only section paths and keys -- disabled or not -- are allowed to be multiline. **/
	INI_ACTIVE_MULTILINE = 2,		/**< Only *active* section paths and *active* keys are allowed to be multiline. **/
	INI_NO_MULTILINE = 3			/**< The multiline escaping sequence is disabled. **/
};

/** @brief	A model format **/
static const IniFormat INI_DEFAULT_FORMAT = _LIBCONFINI_DEFAULT_FORMAT_;

/** @brief	If set to any non-zero value key and section names in case-insensitive INI formats will be dispatched lowercase, verbatim otherwise (default value: non-zero) **/
extern _Bool INI_INSENSITIVE_LOWERCASE;

/** @brief	Value to be dispatched in case of implicit keys (default value: `NULL`) **/
extern char *INI_IMPLICIT_VALUE;

/** @brief	Length of the value dispatched in case of implicit keys -- it can be set to any unsigned number, independently of #INI_IMPLICIT_VALUE (default value: `0`) **/
extern size_t INI_IMPLICIT_V_LEN;



/* CLEAN THE PRIVATE ENVIRONMENT */


#undef _LIBCONFINI_FORMAT_AS_DECLARATION_
#undef _LIBCONFINI_FORMAT_AS_ASSIGNEMENT_
#undef _LIBCONFINI_FORMAT_STRUCT_
#undef _LIBCONFINI_DEFAULT_FORMAT_



/* END OF _LIBCONFINI_HEADER_ */


#endif



/* EOF */

