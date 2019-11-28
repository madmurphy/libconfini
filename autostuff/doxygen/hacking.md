Hacking libconfini {#hacking}
=============================

The following guide illustrates possible changes to the library's code to meet
particular particular needs. For any questions, please [drop a message][1].


## Parsing numbers without the C Standard Library

As explained in the @ref libconfini (see section _§ Compiling for extreme
platforms_), compiling **libconfini** without the C Standard Library
requires getting rid of the functions `ini_get_int()`, `ini_get_lint()`,
`ini_get_llint()` and `ini_get_float()` -- as these are nothing else but
links to the standard functions [`atoi()`][2], [`atol()`][3], [`atoll()`][4]
and [`atof()`][5].

Instead of just removing the function pointers above, however, it is also
possible to provide novel functions implemented from scratch for parsing
numbers. This guide shows possible re-implementations of these functions. Feel
free to make the changes that you wish.

In order to proceed, replace in `src/confini.h` the function pointers
declared immediately after the comment `/*  PUBLIC LINKS  */` with the
following function headers:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
extern int ini_get_int (
    const char * const ini_string
);


extern long int ini_get_lint (
    const char * const ini_string
);


extern long long int ini_get_llint (
    const char * const ini_string
);


extern float ini_get_float (
    const char * const ini_string
);


extern double ini_get_double (
    const char * const ini_string
);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Then replace in `src/confini.c` (at the end of the file) the same
aforementioned pointers with the code below. Note that the C language does
not possess a templating mechanism, so we need to rely on a macro for not
repeating five times the same function body.

_**Note:** The code below is released under the terms of the GPL license,
version 3 or any later version._

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define __PP_IIF__(CONDITION, ...) __PP_EVALCAT__(__PP_UCAT__(__COND, CONDITION), __)(__VA_ARGS__, , )
#define __COND_0__(IF_TRUE, IF_FALSE, ...) IF_FALSE
#define __COND_1__(IF_TRUE, ...) IF_TRUE

#define _LIBCONFINI_RADIX_POINT_ '.'
 
/*
 
Mask `abcd` (6 bits used):
 
    FLAG_1      Continue the loop
    FLAG_2      The numerical part has been found
    FLAG_4      This is the sign
    FLAG_8      Radix point has not been encountered yet
    FLAG_16     This is a digit
    FLAG_32     The number is negative
 
*/
#define _LIBCONFINI_STR2NUM_FNBODY_(IS_RATIONAL, DATA_TYPE, STR) \
    register uint8_t abcd = 9; \
    register DATA_TYPE retval = 0; \
    __PP_IIF__(IS_RATIONAL, \
        DATA_TYPE fact = 1; \
    ) \
    for (register size_t idx = 0; abcd & 1; idx++) { \
        abcd    =   STR[idx] == '-' ? \
                        (abcd & 6 ? abcd & 14 : (abcd & 47) | 36) \
                    : STR[idx] == '+' ? \
                        (abcd & 6 ? abcd & 14 : (abcd & 15) | 4) \
                    : STR[idx] > 47 && STR[idx] < 58 ? \
                        abcd | 18 \
                    __PP_IIF__(IS_RATIONAL, \
                    : (abcd & 8) && STR[idx] == _LIBCONFINI_RADIX_POINT_ ? \
                        (abcd & 39) | 2 \
                    ) \
                    : !(abcd & 2) && is_some_space(STR[idx], _LIBCONFINI_WITH_EOL_) ? \
                        (abcd & 47) | 1 \
                    : \
                        abcd & 46; \
        if (abcd & 16) { \
            retval =    __PP_IIF__(IS_RATIONAL, \
                        abcd & 8 ? \
                            STR[idx] - 48 + retval * 10 \
                        : \
                            (STR[idx] - 48) / (fact *= 10) + retval, \
                        retval * 10 + STR[idx] - 48 \
                        ); \
        } \
    } \
    return abcd & 32 ? -retval : retval;
 
#define __INTEGER_DATA_TYPE__ 0
#define __FLOAT_DATA_TYPE__ 1
 
int ini_get_int (const char * const ini_string) {
    _LIBCONFINI_STR2NUM_FNBODY_(__INTEGER_DATA_TYPE__, int, ini_string);
}
 
long int ini_get_lint (const char * const ini_string) {
    _LIBCONFINI_STR2NUM_FNBODY_(__INTEGER_DATA_TYPE__, long int, ini_string);
}
 
long long int ini_get_llint (const char * const ini_string) {
    _LIBCONFINI_STR2NUM_FNBODY_(__INTEGER_DATA_TYPE__, long long int, ini_string);
}
 
float ini_get_float (const char * const ini_string) {
    _LIBCONFINI_STR2NUM_FNBODY_(__FLOAT_DATA_TYPE__, float, ini_string);
}
 
double ini_get_double (const char * const ini_string) {
    _LIBCONFINI_STR2NUM_FNBODY_(__FLOAT_DATA_TYPE__, double, ini_string);
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## Using a non-standard I/O API

Currently only two I/O standards are supported: the C Standard and POSIX
Standard. If you think that **libconfini** should also support a I/O API
provided by a non-standard header (such as `fsio.h`, `mem-ffs.h`, `efs.h` or
`ff.h`), please do not hesitate to [propose it][1].


[1]: https://github.com/madmurphy/libconfini/issues
[2]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi
[3]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol
[4]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll
[5]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof

