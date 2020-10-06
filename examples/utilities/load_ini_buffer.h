/*  examples/utilities/load_ini_buffer.h  */

#include <stdio.h>
#include <string.h>
#include <confini.h>

/**

  @brief    Parse a `const` string containing an INI file
  @param    ini_buffer      The buffer containing the INI file to
                            parse
  @param    ini_length      The length of @p ini_buffer
  @param    format          The format of the INI file
  @param    f_init          The function that will be invoked before
                            the first dispatch, or `NULL`

  @param    f_foreach       The function that will be invoked for
                            each dispatch, or `NULL`
  @param    user_data       A custom argument, or `NULL`
  @return   Zero for success, otherwise an error code (see `enum`
            #ConfiniInterruptNo)

**/
int load_ini_buffer (
  const char * const ini_buffer,
  const size_t ini_length,
  const IniFormat format,
  const IniStatsHandler f_init,
  const IniDispHandler f_foreach,
  void * const user_data
) {

  char * const ini_cache = strndup(ini_buffer, ini_length);

  if (!ini_cache) {

    return CONFINI_ENOMEM;

  }

  const int retval = strip_ini_cache(
    ini_cache,
    ini_length,
    format,
    f_init,
    f_foreach,
    user_data
  );

  free(ini_cache);

  return retval;

}

