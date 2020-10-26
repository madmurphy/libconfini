/*  -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ !START_OMISSION! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@-

This is a marked **exact copy** of `src/confini.h`, in which replaceable
sections have been wrapped within special tags that can be parsed and amended
by GNU Make during the build process in order to create custom forks of the
library.

If you want to contribute to the development of this project, please **use this
file**, as `src/confini.h` is automatically generated from here.

For more information about the tags used here, please see the `NA_AMEND()`
macro in `m4/not-autotools.m4` at https://github.com/madmurphy/not-autotools

The code below is distributed under the terms of the GPL license version 3 or
any later version.

-@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ !END_OMISSION! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*@@@@@@@@@@@@@@@@@@@@ !ENTRY_POINT(CONFINI_H_METADATA)! @@@@@@@@@@@@@@@@@@@@*/


#ifndef _LIBCONFINI_HEADER_
#define _LIBCONFINI_HEADER_



/*@@@@@@@@@@@@@@@@@@@ !START_EXCEPTION(STANDARD_HEADERS)! @@@@@@@@@@@@@@@@@@@*/
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
/*@@@@@@@@@@@@@@@@@@@@ !END_EXCEPTION(STANDARD_HEADERS)! @@@@@@@@@@@@@@@@@@@@*/



#ifdef __cplusplus
extern "C" {
#endif



/*  PRIVATE (HEADER-SCOPED) MACROS  */


#ifdef _LIBCONFINI_NOCCWARN_
#define _LIBCONFINI_DO_PRAGMA_(PBODY)
#define _LIBCONFINI_WARNING_(MSG)
#elif defined(_MSC_VER)
#define _LIBCONFINI_DO_PRAGMA_(PBODY) __pragma(PBODY)
#define _LIBCONFINI_WARNING_(MSG) _LIBCONFINI_DO_PRAGMA_(message("*WARNING*: " MSG))
#else
#define _LIBCONFINI_DO_PRAGMA_(PBODY) _Pragma(#PBODY)
#define _LIBCONFINI_WARNING_(MSG) _LIBCONFINI_DO_PRAGMA_(GCC warning #MSG)
#endif

#define __INIFORMAT_TABLE_CB_FIELDS__(NAME, OFFSET, SIZE, DEFVAL) \
    unsigned char NAME:SIZE;
#define __INIFORMAT_TABLE_CB_DEFAULT__(NAME, OFFSET, SIZE, DEFVAL) DEFVAL,
#define __INIFORMAT_TABLE_CB_ZERO__(NAME, OFFSET, SIZE, DEFVAL) 0,
#define _LIBCONFINI_INIFORMAT_TYPE_ \
    struct IniFormat { INIFORMAT_TABLE_AS(__INIFORMAT_TABLE_CB_FIELDS__) }
#define _LIBCONFINI_DEFAULT_FORMAT_ \
    { INIFORMAT_TABLE_AS(__INIFORMAT_TABLE_CB_DEFAULT__) }
#define _LIBCONFINI_UNIXLIKE_FORMAT_ \
    { INIFORMAT_TABLE_AS(__INIFORMAT_TABLE_CB_ZERO__) }



/*  PUBLIC MACROS  */


/**
    @brief  Call a user-given macro (that accepts four arguments) for each row
            of the table
**/
/*
    NOTE: The following table and the order of its rows **define** (and link
    together) both the #IniFormat and #IniFormatNum data types declared in this
    header
*/
#define INIFORMAT_TABLE_AS(_____)                 /*  IniFormat table  *\

        NAME                      BIT  SIZE DEFAULT
                                                                      */\
 _____( delimiter_symbol,         0,   7,   INI_EQUALS                ) \
 _____( case_sensitive,           7,   1,   false                     )/*
                                                                      */\
 _____( semicolon_marker,         8,   2,   INI_DISABLED_OR_COMMENT   ) \
 _____( hash_marker,              10,  2,   INI_DISABLED_OR_COMMENT   ) \
 _____( section_paths,            12,  2,   INI_ABSOLUTE_AND_RELATIVE ) \
 _____( multiline_nodes,          14,  2,   INI_MULTILINE_EVERYWHERE  )/*
                                                                      */\
 _____( no_single_quotes,         16,  1,   false                     ) \
 _____( no_double_quotes,         17,  1,   false                     ) \
 _____( no_spaces_in_names,       18,  1,   false                     ) \
 _____( implicit_is_not_empty,    19,  1,   false                     ) \
 _____( do_not_collapse_values,   20,  1,   false                     ) \
 _____( preserve_empty_quotes,    21,  1,   false                     ) \
 _____( disabled_after_space,     22,  1,   false                     ) \
 _____( disabled_can_be_implicit, 23,  1,   false                     )



/**
    @brief  Check whether a format does _not_ support escape sequences
**/
#define INIFORMAT_HAS_NO_ESC(FORMAT) \
    (FORMAT.multiline_nodes == INI_NO_MULTILINE && \
    FORMAT.no_double_quotes && FORMAT.no_single_quotes)



/**
    @brief  Check whether a given `char *` data type points to the global
            variable #INI_GLOBAL_IMPLICIT_VALUE or to any fragment of it
**/
#define INI_IS_IMPLICIT_SUBSTR(CHAR_PTR) \
    (CHAR_PTR >= INI_GLOBAL_IMPLICIT_VALUE && CHAR_PTR <= \
    INI_GLOBAL_IMPLICIT_VALUE + INI_GLOBAL_IMPLICIT_V_LEN)



/*  PUBLIC TYPEDEFS  */


/**
    @brief  24-bit bitfield representing the format of an INI file (INI
            dialect)
**/
typedef _LIBCONFINI_INIFORMAT_TYPE_ IniFormat;


/**
    @brief  Global statistics about an INI file
**/
typedef struct IniStatistics {
    const IniFormat format;
    const size_t bytes;
    const size_t members;
} IniStatistics;


/**
    @brief  Dispatch of a single INI node
**/
typedef struct IniDispatch {
    const IniFormat format;
    uint_least8_t type;
    char * data;
    char * value;
    const char * append_to;
    size_t d_len;
    size_t v_len;
    size_t at_len;
    size_t dispatch_id;
} IniDispatch;


/**
    @brief  The unique ID of an INI format (24-bit maximum)
**/
typedef uint_least32_t IniFormatNum;


/**
    @brief  Callback function for handling an #IniStatistics structure
**/
typedef int (* IniStatsHandler) (
    IniStatistics * statistics,
    void * user_data
);


/**
    @brief  Callback function for handling an #IniDispatch structure
**/
typedef int (* IniDispHandler) (
    IniDispatch * dispatch,
    void * user_data
);


/**
    @brief  Callback function for handling an INI string belonging to a
            sequence of INI strings
**/
typedef int (* IniStrHandler) (
    char * ini_string,
    size_t string_length,
    size_t string_num,
    IniFormat format,
    void * user_data
);


/**
    @brief  Callback function for handling a selected fragment of an INI string
**/
typedef int (* IniSubstrHandler) (
    const char * ini_string,
    size_t fragm_offset,
    size_t fragm_length,
    size_t fragm_num,
    IniFormat format,
    void * user_data
);



/*  PUBLIC FUNCTIONS  */


extern int strip_ini_cache (
    register char * const ini_source,
    const size_t ini_length,
    const IniFormat format,
    const IniStatsHandler f_init,
    const IniDispHandler f_foreach,
    void * const user_data
);

/*@@@@@@@@@@@@@@@@@@@@@ !START_EXCEPTION(IO_FUNCTIONS)! @@@@@@@@@@@@@@@@@@@@@*/

extern int load_ini_file (
    FILE * const ini_file,
    const IniFormat format,
    const IniStatsHandler f_init,
    const IniDispHandler f_foreach,
    void * const user_data
);


extern int load_ini_path (
    const char * const path,
    const IniFormat format,
    const IniStatsHandler f_init,
    const IniDispHandler f_foreach,
    void * const user_data
);

/*@@@@@@@@@@@@@@@@@@@@@@ !END_EXCEPTION(IO_FUNCTIONS)! @@@@@@@@@@@@@@@@@@@@@@*/

extern bool ini_string_match_ss (
    const char * const simple_string_a,
    const char * const simple_string_b,
    const IniFormat format
);


extern bool ini_string_match_si (
    const char * const simple_string,
    const char * const ini_string,
    const IniFormat format
);


extern bool ini_string_match_ii (
    const char * const ini_string_a,
    const char * const ini_string_b,
    const IniFormat format
);


extern bool ini_array_match (
    const char * const ini_string_a,
    const char * const ini_string_b,
    const char delimiter,
    const IniFormat format
);


extern size_t ini_unquote (
    char * const ini_string,
    const IniFormat format
);


extern size_t ini_string_parse (
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
    const IniSubstrHandler f_foreach,
    void * const user_data
);


extern size_t ini_array_shift (
    const char ** const ini_strptr,
    const char delimiter,
    const IniFormat format
);


extern size_t ini_array_collapse (
    char * const ini_string,
    const char delimiter,
    const IniFormat format
);


extern char * ini_array_break (
    char * const ini_string,
    const char delimiter,
    const IniFormat format
);


extern char * ini_array_release (
    char ** const ini_strptr,
    const char delimiter,
    const IniFormat format
);


extern int ini_array_split (
    char * const ini_string,
    const char delimiter,
    const IniFormat format,
    const IniStrHandler f_foreach,
    void * const user_data
);


extern void ini_global_set_lowercase_mode (
    const bool lowercase
);
#define ini_global_set_lowercase_mode \
    _LIBCONFINI_WARNING_("deprecated function `ini_global_set_lowercase_mode()`") \
    ini_global_set_lowercase_mode


extern void ini_global_set_implicit_value (
    char * const implicit_value,
    const size_t implicit_v_len
);


extern IniFormatNum ini_fton (
    const IniFormat format
);


extern IniFormat ini_ntof (
    const IniFormatNum format_id
);


extern int ini_get_bool (
    const char * const simple_string,
    const int when_fail
);


extern int ini_get_bool_i (
    const char * const ini_string,
    const int when_fail,
    const IniFormat format
);


/*@@@@@@@@@@@@@@@@@@@@@ !START_EXCEPTION(LIBC_STR2NUM)! @@@@@@@@@@@@@@@@@@@@@*/

/*  PUBLIC LINKS  */


extern int (* const ini_get_int) (
    const char * ini_string
);


extern long int (* const ini_get_lint) (
    const char * ini_string
);


extern long long int (* const ini_get_llint) (
    const char * ini_string
);


extern double (* const ini_get_double) (
    const char * ini_string
);


/*
    Legacy support -- please **do not use these functions**!
*/
#define ini_get_float \
    _LIBCONFINI_WARNING_("function `ini_get_float()` is deprecated for parsing a `double` data type; use `ini_get_double()` instead") \
    ini_get_double
/*@@@@@@@@@@@@@@@@@@@@@@ !END_EXCEPTION(LIBC_STR2NUM)! @@@@@@@@@@@@@@@@@@@@@@*/



/*  PUBLIC CONSTANTS AND VARIABLES  */


/**
    @brief  Error mask (flags not present in user-generated interruptions)
**/
#define CONFINI_ERROR 252


/**
    @brief  Error codes
**/
enum ConfiniInterruptNo {
    CONFINI_SUCCESS = 0,    /**< There have been no interruptions, everything
                                 went well [value=0] **/
    CONFINI_IINTR = 1,      /**< Interrupted by the user during `f_init()`
                                 [value=1] **/
    CONFINI_FEINTR = 2,     /**< Interrupted by the user during `f_foreach()`
                                 [value=2] **/
    CONFINI_ENOENT = 4,     /**< File inaccessible [value=4] **/
    CONFINI_ENOMEM = 5,     /**< Error allocating virtual memory [value=5] **/
    CONFINI_EIO = 6,        /**< Error reading the file [value=6] **/
    CONFINI_EOOR = 7,       /**< Out-of-range error: callbacks are more than
                                 expected [value=7] **/
    CONFINI_EBADF = 8,      /**< The stream specified is not a seekable stream
                                 [value=8] **/
    CONFINI_EFBIG = 9,      /**< File too large [value=9] **/
    CONFINI_EROADDR = 10    /**< Address is read-only [value=10] **/
};


/**
    @brief  Disabled flag (i.e. third bit, present only in non-active node
            types)
**/
#define INI_DISABLED_FLAG 4


/**
    @brief  INI node types and possible values of #IniDispatch::type

    See also #INI_DISABLED_FLAG.
**/
enum IniNodeType {
    INI_UNKNOWN = 0,            /**< This is a node impossible to categorize
                                     [value=0] **/
    INI_VALUE = 1,              /**< Not used by **libconfini** (values are
                                     dispatched together with keys) -- but
                                     available for user's implementations
                                     [value=1] **/
    INI_KEY = 2,                /**< This is a key [value=2] **/
    INI_SECTION = 3,            /**< This is a section path [value=3] **/
    INI_COMMENT = 4,            /**< This is a comment [value=4] **/
    INI_INLINE_COMMENT = 5,     /**< This is an inline comment [value=5] **/
    INI_DISABLED_KEY = 6,       /**< This is a disabled key [value=6] **/
    INI_DISABLED_SECTION = 7    /**< This is a disabled section path
                                     [value=7] **/
};


/**
    @brief  Common array and key-value delimiters (but a delimiter may also be
            any other ASCII character not present in this list)
**/
enum IniDelimiters {
    INI_ANY_SPACE = 0,  /**< In multi-line INIs:
                             `/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`, in
                             non-multi-line INIs: `/[\t \v\f])+/` **/
    INI_EQUALS = '=',   /**< Equals character (`=`) **/
    INI_COLON = ':',    /**< Colon character (`:`) **/
    INI_DOT = '.',      /**< Dot character (`.`) **/
    INI_COMMA = ','     /**< Comma character (`,`) **/
};


/**
    @brief  Possible values of #IniFormat::semicolon_marker and
            #IniFormat::hash_marker (i.e., meaning of `/\s+;/` and `/\s+#/` in
            respect to a format)
**/
enum IniCommentMarker {
    INI_DISABLED_OR_COMMENT = 0,    /**< This marker opens a comment or a
                                         disabled entry **/
    INI_ONLY_COMMENT = 1,           /**< This marker opens a comment **/
    INI_IGNORE = 2,                 /**< This marker opens a comment that has
                                         been marked for deletion and must not
                                         be dispatched or counted **/
    INI_IS_NOT_A_MARKER = 3         /**< This is not a marker at all, but a
                                         normal character instead **/
};


/**
    @brief  Possible values of #IniFormat::section_paths
**/
enum IniSectionPaths {
    INI_ABSOLUTE_AND_RELATIVE = 0,  /**< Section paths starting with a dot
                                         express nesting to the current parent,
                                         to root otherwise **/
    INI_ABSOLUTE_ONLY = 1,          /**< Section paths starting with a dot will
                                         be cleaned of their leading dot and
                                         appended to root **/
    INI_ONE_LEVEL_ONLY = 2,         /**< Format supports sections, but the dot
                                         does not express nesting and is not a
                                         meta-character **/
    INI_NO_SECTIONS = 3             /**< Format does *not* support sections --
                                         `/\[[^\]]*\]/g`, if any, will be
                                         treated as keys! **/
};


/**
    @brief  Possible values of #IniFormat::multiline_nodes
**/
enum IniMultiline {
    INI_MULTILINE_EVERYWHERE = 0,       /**< Comments, section paths and keys
                                             -- disabled or not -- are allowed
                                             to be multi-line **/
    INI_BUT_COMMENTS = 1,               /**< Only section paths and keys --
                                             disabled or not -- are allowed to
                                             be multi-line **/
    INI_BUT_DISABLED_AND_COMMENTS = 2,  /**< Only active section paths and
                                             active keys are allowed to be
                                             multi-line **/
    INI_NO_MULTILINE = 3                /**< Multi-line escape sequences are
                                             disabled **/
};


/**
    @brief  A model format for standard INI files
**/
static const IniFormat INI_DEFAULT_FORMAT = _LIBCONFINI_DEFAULT_FORMAT_;


/**
    @brief  A model format for Unix-like .conf files (where space characters
            are delimiters between keys and values)
**/
/*  All fields are set to `0` here.  */
static const IniFormat INI_UNIXLIKE_FORMAT = _LIBCONFINI_UNIXLIKE_FORMAT_;


/**

    @brief          If set to `true`, key and section names in case-insensitive
                    INI formats will be dispatched lowercase, verbatim
                    otherwise (default value: `false`)
    @deprecated     Deprecated since version 1.15.0 (it will be removed in
                    version 2.0.0)
**/
extern bool INI_GLOBAL_LOWERCASE_MODE;
#define INI_GLOBAL_LOWERCASE_MODE \
    _LIBCONFINI_WARNING_("global variable `INI_GLOBAL_LOWERCASE_MODE` is deprecated") \
    INI_GLOBAL_LOWERCASE_MODE


/**
    @brief  Value to be assigned to implicit keys (default value: `NULL`)
**/
extern char * INI_GLOBAL_IMPLICIT_VALUE;


/**
    @brief  Length of the value assigned to implicit keys (default value: `0`)
**/
extern size_t INI_GLOBAL_IMPLICIT_V_LEN;



/*  CLEAN THE PRIVATE ENVIRONMENT  */


#undef _LIBCONFINI_UNIXLIKE_FORMAT_
#undef _LIBCONFINI_DEFAULT_FORMAT_
#undef _LIBCONFINI_INIFORMAT_TYPE_
#undef __INIFORMAT_TABLE_CB_ZERO__
#undef __INIFORMAT_TABLE_CB_DEFAULT__
#undef __INIFORMAT_TABLE_CB_FIELDS__



/*  END OF `_LIBCONFINI_HEADER_`  */


#ifdef __cplusplus
}
#endif


#endif


/*  EOF  */

