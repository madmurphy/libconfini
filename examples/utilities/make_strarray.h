/*  examples/utilities/make_strarray.h  */

#include <stdlib.h>
#include <string.h>
#include <confini.h>

/**

  @brief    Converts a stringified INI array to a new array of strings
  @param    dest_arrlen     The variable where the length of the array (in
                            number of members) will be saved (cannot be
                            `NULL`)
  @param    str             The stringified array (it cannot be `NULL`)
  @param    len             The length of the stringified array in bytes
  @param    delimiter       The delimiter between the array members -- if zero
                            (see #INI_ANY_SPACE), any space is delimiter
                            (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
  @param    format          The format of the INI file
  @return   A new array of char pointers

  Example:

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
  size_t my_arrlen;
  char ** my_array = make_strarray(
    this->value,
    this->v_len,
    INI_ANY_SPACE,
    this->format,
    &my_arrlen
  );

  // Do something with `my_array`...

  free(my_array);
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  The returned address (and **only that**) must be freed afterwards.

**/
static char ** make_strarray (
  size_t * const dest_arrlen,
  const char * const str,
  const size_t len,
  const char delimiter,
  const IniFormat format
) {

  *dest_arrlen = ini_array_get_length(str, delimiter, format);

  char ** const
    newarr  =   *dest_arrlen ?
                  (char **) malloc(*dest_arrlen * sizeof(char *) + len + 1)
                :
                  NULL;


  if (!newarr) {

    return NULL;

  }

  memcpy(newarr + *dest_arrlen, str, len + 1);

  char * iter = (char *) ((char **) newarr + *dest_arrlen);

  for (size_t idx = 0; idx < *dest_arrlen; idx++) {

    newarr[idx] = ini_array_release(&iter, delimiter, format);
    ini_string_parse(newarr[idx], format);

  }

  return newarr;

}

