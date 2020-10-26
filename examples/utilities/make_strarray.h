/*  examples/utilities/make_strarray.h  */

#include <stdlib.h>
#include <string.h>
#include <confini.h>

/**

  @brief    Convert a stringified INI array to a new array of strings
  @param    dest_arrlen     The variable where the length of the array (in
                            number of members) will be saved (cannot be
                            `NULL`)
  @param    ini_string      The stringified array (it cannot be `NULL`)
  @param    len             The length of the stringified array in bytes
  @param    delimiter       The delimiter between the array members -- if zero
                            (see #INI_ANY_SPACE), any space is delimiter
                            (`/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`)
  @param    format          The format of the INI file
  @return   A new array of char pointers

  The returned address (and **only that**) must be freed afterwards.

  Example usage:

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
  size_t my_arrlen;

  char ** my_array = make_strarray(
    &my_arrlen,
    disp->value,
    disp->v_len,
    INI_ANY_SPACE,
    disp->format
  );

  if (my_arrlen && !my_array) {
    fprintf(stderr, "malloc() failed");
    exit(1);
  }

  // Do something with `my_array`...

  if (my_array) {
    free(my_array);
  }
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**/
static char ** make_strarray (
  size_t * const dest_arrlen,
  const char * const ini_string,
  const size_t len,
  const char delimiter,
  const IniFormat format
) {

  *dest_arrlen = ini_array_get_length(ini_string, delimiter, format);

  char ** const
    newarr  =   *dest_arrlen ?
                  (char **) malloc(*dest_arrlen * sizeof(char *) + len + 1)
                :
                  NULL;

  if (!newarr) {
    return NULL;
  }

  char * remnant = (char *) ((char **) newarr + *dest_arrlen);
  memcpy(remnant, ini_string, len + 1);
  for (size_t idx = 0; idx < *dest_arrlen; idx++) {
    newarr[idx] = ini_array_release(&remnant, delimiter, format);
    ini_string_parse(newarr[idx], format);
  }

  return newarr;

}

