Compiling without I/O {#baremetal}
==================================

Almost everything in **libconfini** is implemented from scratch, with the only
notable exception of the I/O functions `load_ini_file()` and `load_ini_path()`,
which rely on standard libraries (either the C Standard or the POSIX Standard,
depending on the build settings). On some platforms, however, only a rather
exotic I/O API is available, while for some other platforms the C Standard
Library is simply too heavy or just not implementable.

In the past, the build environment of **libconfini** did not offer shortcuts
for facing this kind of situations -- although, thanks to the modularity of the
source code, it was still relatively simple to get rid of every tie with the C
Standard Library and compile **libconfini** as “bare metal”, with
`strip_ini_cache()` as the only parsing function (since this relies only on a
buffer for its input), i.e. without `load_ini_file()` and `load_ini_path()`,
and possibly even without any header at all.

Starting from version 1.13.0 a “bare metal” version of **libconfini** has been
made available simply by passing a `--without-io-api` option to the `configure`
script. This modified version presents the following characteristics:

* No heap usage (no memory is ever allocated or freed)
* No I/O functions (it is possible to parse only disposable `char` buffers via
  `strip_ini_cache()`)
* Everything else is inherited verbatim from the official version


## The `dev/hackings/baremetal/` subdirectory

When the `configure` script is launched with the `--without-io-api` option (or,
equivalently, with `--with-io-api=baremetal`), it assumes that no standard
library at all could be present in the system. Hence it runs a series of tests
and creates an inventory of what is present and what is not, in order to amend
the source code accordingly -- to ignore all the tests and assume that
literally nothing from the C Standard Library is supported use
`--without-libc`. The amendments are necessary (instead of just relying on the
C preprocessor) because it is required to change the public header, not just
the compiled code.

Only a very small amount of code in **libconfini** depends on the C Standard
Library besides the I/O functions, so it is relatively easy to produce a “bare
metal” fork with or without the latter. The `dev/hackings/baremetal`
subdirectory contains all the necessary amendments. These are automatically
applied when launching `make all` or `make baremetal-source-code` after having
launched `./configure --without-io-api` (the original source code will be
preserved).

The files `pp-utils.c` and `str2num.c` constitute a re-implementation of the
functions `ini_get_int()`, `ini_get_lint()`, `ini_get_llint()`,
`ini_get_float()` and `ini_get_double()`, which in the original code are
implemented as pointers to standard functions (see below). These two files
amend `src/confini.c`.

The file `str2num.h`, which amends `src/confini.h` (i.e. the public header),
exports the function headers of what `str2num.c` implements.

The file `confini-header.c` contains only a nominal workaround-amendment to
`src/confini.c` (for facilitating the build system) that does not change the
final C code compiled.

To create the source code of a “bare metal” version of **libconfini** a fifth
amendment to the public header is also required, containing some common C
standard definitions. This amendment is automatically generated for each
platform during the build process and will be located under
`no-dist/hackings/baremetal/c-standard-library.h`.

Here follows the summary of what is required by `./configure --without-io-api`:

1. `dev/hackings/baremetal/confini-header.c` (pasted to the private module
   `src/confini.c`)
2. `dev/hackings/baremetal/str2num.c` (pasted to the private module
   `src/confini.c`)
3. `dev/hackings/baremetal/str2num.h` (pasted to the public header
   `src/confini.h`)
4. `dev/hackings/baremetal/pp-utils.c` (pasted to the private module
   `src/confini.c`)
5. `no-dist/hackings/baremetal/c-standard-library.h` (pasted to the public
   header `src/confini.h` after having been automaticaly generated either by
   the `configure` script, as an exact copy of
   `dev/hackings/baremetal/c-standard-library.h`, or by `make
   approve-revision`, in the few cases where manual user's intervention is
   required during the build process)

The first four files (the ones located in the `dev/hackings/baremetal`
subdirectory) are static and do not need any intervention from the user, unless
(s)he wants to participate in the development of **libconfini**. The fifth file
_might_ require manual intervention in some situations, depending on the
platform or on the user's choice (the build system will emit a warning in such
cases).


## Re-implementing `load_ini_file()` and `load_ini_path()` for your platform

Whether you are using the bare-metal version of **libconfini** for a regular
computer, a fridge or a microwave oven, you might have eventually to deal with
some kind of filesystem. If the C standard `fopen()`, `fseek()`, `ftell()`,
`rewind()`, `fread()` and `fclose()` do not suit your needs, you can
re-implement your own version of `load_ini_file()` and `load_ini_path()`. The
only requirement is that at the end of the day you find a way to pass a
disposable buffer containing an entire INI file to `strip_ini_cache()`. A good
way to proceed is to hack the original pair of functions that rely on the C
standard I/O API and adapt them to your platform.

`````````````````````````````````````````````````````````````````````````` c
#include <stdio.h>
#include <stdlib.h>

int load_ini_file (
  FILE * const ini_file,
  const IniFormat format,
  const IniStatsHandler f_init,
  const IniDispHandler f_foreach,
  void * const user_data
) {

  long int file_size;

  if (fseek(ini_file, 0, SEEK_END) || (file_size = ftell(ini_file)) < 0) {

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

  long int file_size;

  if (fseek(ini_file, 0, SEEK_END) || (file_size = ftell(ini_file)) < 0) {

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
``````````````````````````````````````````````````````````````````````````


## Going manual

If instead of using the `--without-io-api` option you prefer to adapt the code
manually, follow these simple steps (or adjust them according to your needs),
which map verbatim what `--without-io-api` does:

1. Remove `#include <stdio.h>` from `confini.h`
2. If the `stddef.h` header (providing the `typedef` for the `size_t` data
   type) is available, add `#include <stddef.h>` to `confini.h`, otherwise
   provide your own implementation of the `size_t` data type
3. If the `stdint.h` header (providing the `typedef` for the `int8_t`,
   `uint8_t`, `uint16_t` and `uint32_t` data types) is not available, remove it
   from `confini.h` and provide your own implementation of the `int8_t`,
   `uint8_t`, `uint16_t` `uint32_t` data types
4. Remove the functions `load_ini_file()` and `load_ini_path()` from both
   `confini.h` and `confini.c`
5. If the `stdlib.h` header is not available, remove `#include <stdlib.h>` from
   `confini.c` and remove the function pointers `ini_get_int`, `ini_get_lint`,
   `ini_get_llint` and `ini_get_double` from both `confini.h` and `confini.c`
   (you will have to use your own functions for converting strings to numbers)
   -- for a set of possible replacements _as actual functions_ instead of
   function pointers, please see below)
6. Leave everything else as it is

After doing so **libconfini** will work even without a kernel.


### Parsing numbers without the C Standard Library

As explained above, compiling **libconfini** without the C Standard Library
requires getting rid of the function pointers `ini_get_int()`,
`ini_get_lint()`, `ini_get_llint()` and `ini_get_double()` -- as these are
nothing but links to the standard functions [`atoi()`][1], [`atol()`][2],
[`atoll()`][3] and [`atof()`][4].

Instead of just removing these function pointers, however, it is also possible
to provide novel functions implemented from scratch for parsing numbers.

What `./configure --without-io-api` does in fact is replacing in
`src/confini.h` the function pointers declared immediately after the comment
`/*  PUBLIC LINKS  */` with the following function headers:

```````````````````````````````````` c
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
````````````````````````````````````

The same `configure` option amends in `src/confini.c` (at the end of the
file) the corresponding pointers with the code below. Note that the C language
does not possess a templating mechanism, so the following code needs to rely on
a macro for not repeating five times the same function body with only minimal
variations.

````````````````````````````````````````````````````````````````````````````` c
/*
  The code below is free software. You can redistribute it and/or modify it
  under the terms of the GPL license version 3 or any later version. See
  COPYING for details. 
*/

#define __PP_IIF__(CONDITION, ...) __PP_EVALUCAT__(__PP_UCAT__(__COND, CONDITION), _)(__VA_ARGS__, , )
#define __COND_0__(IF_TRUE, IF_FALSE, ...) IF_FALSE
#define __COND_1__(IF_TRUE, ...) IF_TRUE

#define _LIBCONFINI_RADIX_POINT_ '.'
#define __INTEGER_DATA_TYPE__ 0
#define __FLOAT_DATA_TYPE__ 1


/*

Mask `abcd` (6 bits used):

  FLAG_1    Continue the loop
  FLAG_2    The numerical part has been found
  FLAG_4    This is the sign
  FLAG_8    Radix point has not been encountered yet
  FLAG_16   This is a digit
  FLAG_32   The number is negative

*/
#define _LIBCONFINI_STR2NUM_FNBODY_(HAS_RADIX_PT, DATA_TYPE, STR) \
  register uint8_t abcd = 9; \
  register size_t idx = 0; \
  register DATA_TYPE retval = 0; \
  __PP_IIF__(HAS_RADIX_PT, \
    DATA_TYPE fact = 1; \
  ) \
  do { \
    abcd  =   STR[idx] > 47 && STR[idx] < 58 ? \
                abcd | 18 \
              : !(abcd & 6) && STR[idx] == '-' ? \
                (abcd & 47) | 36 \
              : !(abcd & 6) && STR[idx] == '+' ? \
                (abcd & 15) | 4 \
              __PP_IIF__(HAS_RADIX_PT, \
              : (abcd & 8) && STR[idx] == _LIBCONFINI_RADIX_POINT_ ? \
                (abcd & 39) | 2 \
              ) \
              : !(abcd & 2) && is_some_space(STR[idx], _LIBCONFINI_WITH_EOL_) ? \
                (abcd & 47) | 1 \
              : \
                abcd & 46; \
    if (abcd & 16) { \
      retval  =   __PP_IIF__(HAS_RADIX_PT, \
                  abcd & 8 ? \
                    STR[idx] - 48 + retval * 10 \
                  : \
                    (STR[idx] - 48) / (fact *= 10) + retval, \
                  retval * 10 + STR[idx] - 48 \
                  ); \
    } \
    idx++; \
  } while (abcd & 1); \
  return abcd & 32 ? -retval : retval;


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
`````````````````````````````````````````````````````````````````````````````


  [1]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi
  [2]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol
  [3]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll
  [4]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof

