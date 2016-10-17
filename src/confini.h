/**

	@file		confini.h
	@brief		libconfini header
	@author		Stefano Gioffr&eacute;
	@copyright 	GNU Public License v3
	@date		2016

**/

#ifndef _LIBCONFINI_HEADER_
#define _LIBCONFINI_HEADER_



/* PRIVATE (HEADER-SCOPED) MACROS */


#define _LIBCONFINI_FORMAT_AS_DECLARATION_(SIZE, PROPERTY, VALUE) unsigned char PROPERTY:SIZE;
#define _LIBCONFINI_FORMAT_AS_ASSIGNEMENT_(SIZE, PROPERTY, VALUE) VALUE,
#define _LIBCONFINI_FORMAT_STRUCT_ struct IniFormat { _LIBCONFINI_EXPAND_FORMAT_MACRO_AS_(_LIBCONFINI_FORMAT_AS_DECLARATION_) }
#define _LIBCONFINI_DEFAULT_FORMAT_ { _LIBCONFINI_EXPAND_FORMAT_MACRO_AS_(_LIBCONFINI_FORMAT_AS_ASSIGNEMENT_) }



/* PUBLIC MACROS */


#define _LIBCONFINI_EXPAND_FORMAT_MACRO_AS_(______)                          /*-*\

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


/** @brief	24-bit bitfield representing a unique format of an INI file (INI dialect) **/
typedef _LIBCONFINI_FORMAT_STRUCT_ IniFormat;

/** @brief	Global statistics about an INI file **/
typedef struct IniStatistics {
	const IniFormat format;
	unsigned long int bytes;
	unsigned long int members;
} IniStatistics;

/** @brief	Dispatch of a single INI member **/
typedef struct IniDispatch {
	const IniFormat format;
	unsigned short int type;
	char *data;
	char *value;
	char *append_to;
	unsigned long int d_length;
	unsigned long int v_length;
	unsigned long int dispatch_id;
} IniDispatch;

/** @brief	24-bit bitmask representing the format of an INI file **/
typedef unsigned long int IniFormatId;



/* PUBLIC FUNCTIONS */


extern unsigned int load_ini_file (
	const char * const path,
	const IniFormat format,
	int (* const f_init)(
		IniStatistics *statistics,
		void *init_other
	),
	int (* const f_foreach)(
		IniDispatch *dispatch,
		void *foreach_other
	),
	void *user_data
);

extern void ini_set_implicit_value (char * const implicit_value, const unsigned long int implicit_v_length);

extern IniFormatId ini_format_get_id (const IniFormat format);

extern void ini_format_set_to_id (IniFormat *dest_format, IniFormatId format_id);

extern unsigned long int ini_unquote (char * const ini_string, const IniFormat format);

extern unsigned long int ini_array_get_length (const char * const ini_string, const char delimiter, const IniFormat format);

extern unsigned int ini_array_foreach (
	const char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach)(
		const char *member,
		unsigned int offset,
		unsigned int length,
		void *foreach_other
	),
	void *user_data
);

extern unsigned int ini_split_array (
	char * const ini_string,
	const char delimiter,
	const IniFormat format,
	int (* const f_foreach)(
		char *element,
		unsigned int length,
		void *foreach_other
	),
	void *user_data
);

extern signed int ini_get_bool (const char * const ini_string, const signed int return_value);

extern signed int ini_get_lazy_bool (const char * const ini_string, const signed int return_value);



/* PUBLIC WRAPPERS */


extern int (* const ini_get_int) (const char *ini_string);

extern long int (* const ini_get_lint) (const char *ini_string);

extern long long int (* const ini_get_llint) (const char *ini_string);

extern double (* const ini_get_float) (const char *ini_string);



/* PUBLIC CONSTANTS AND VARIABLES */


/** @brief	Error codes **/
enum ConfiniErrorNo {
	CONFINI_EFEINTR = 1,	/** Interrupted by the user during f_foreach **/
	CONFINI_EFEOOR = 2,	/** The loop is longer than expected (out of range) **/
	CONFINI_EIINTR = 3,	/** Interrupted by the user during f_init **/
	CONFINI_ENOMEM = 4,	/** Error allocating memory **/
	CONFINI_EIO = 5,	/** Error reading the file **/
	CONFINI_ENOENT = 6	/** File inaccessible **/
};

/** @brief	Most used delimiters (but a delimiter can also be any other ASCII character) **/
enum IniDelimiters {
	INI_ANY_SPACE = '\0',
	INI_EQUALS = '=',
	INI_COLON = ':'
};

/** @brief	INI nodes types **/
enum IniNodeType {
	INI_UNKNOWN = 0,
	INI_DOCUMENT = 1,
	INI_SECTION = 2,
	INI_KEY = 3,
	INI_COMMENT = 4,
	INI_INLINE_COMMENT = 5,
	INI_DISABLED_SECTION = 6,
	INI_DISABLED_KEY = 7
};

/** @brief	Behaviors of '#' and ';' **/
enum IniComments {
	INI_PARSE_COMMENT = 0,
	INI_SHOW_COMMENT = 1,
	INI_ERASE_COMMENT = 2,
	INI_NORMAL_CHARACTER = 3
};

/** @brief	Multiline entries **/
enum IniMultiline {
	INI_EVERYTHING_MULTILINE = 0,
	INI_ACTIVE_AND_DISABLED_MULTILINE = 1,
	INI_ACTIVE_MULTILINE = 2,
	INI_NO_MULTILINE = 3
};

/** @brief	A model format **/
static const IniFormat INI_DEFAULT_FORMAT = _LIBCONFINI_DEFAULT_FORMAT_;

/** @brief	Default value of implicit keys **/
extern char *INI_IMPLICIT_VALUE;

/** @brief	Default length of implicit keys' value -- it can be set to any unsigned number, independently of #INI_IMPLICIT_VALUE **/
extern unsigned long int INI_IMPLICIT_V_LENGTH;



/* CLEAN THE PRIVATE ENVIRONMENT */


#undef _LIBCONFINI_FORMAT_AS_DECLARATION_
#undef _LIBCONFINI_FORMAT_AS_ASSIGNEMENT_
#undef _LIBCONFINI_FORMAT_STRUCT_
#undef _LIBCONFINI_DEFAULT_FORMAT_



/* END OF _LIBCONFINI_HEADER_ */


#endif



/* EOF */

