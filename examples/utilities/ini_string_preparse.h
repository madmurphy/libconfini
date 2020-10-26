/*  examples/utilities/ini_string_preparse.h  */

#include <confini.h>

/**

  @brief    Extend the supported escape sequences
  @param    ini_string      The INI string (it cannot be `NULL`)
  @return   The new length of the string

  Supported escape sequences are "\a", "\b", "\f", "\n", "\r",
  "\t", "\v", "\e". Double backslashes are not unescaped here.

  Example usage:

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
  #include <confini.h>
  #include "examples/utilities/ini_string_preparse.h"

  static int my_callback (
    IniDispatch * disp,
    void * user_data
  ) {

    if (disp->type == INI_KEY) {

      ini_string_preparse(disp->value);
      ini_string_parse(disp->value, disp->format);

      // DO SOMETHING

    }

    return 0;

  }
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**/
size_t ini_string_preparse (char * const ini_string) {

  if (INI_IS_IMPLICIT_SUBSTR(ini_string)) {

    return INI_GLOBAL_IMPLICIT_V_LEN +
      INI_GLOBAL_IMPLICIT_VALUE - ini_string;

  }

  register char chr, * src = ini_string, * dst = ini_string;

  is_unesc: switch (chr = *src++) {

    case '\\': goto is_esc;
    case '\0': *dst = '\0'; return dst - ini_string;
    default: *dst++ = chr; goto is_unesc;

  }

  is_esc: switch (chr = *src++) {

    case 'a': *dst++ = '\a'; goto is_unesc;
    case 'b': *dst++ = '\b'; goto is_unesc;
    case 'f': *dst++ = '\f'; goto is_unesc;
    case 'n': *dst++ = '\n'; goto is_unesc;
    case 'r': *dst++ = '\r'; goto is_unesc;
    case 't': *dst++ = '\t'; goto is_unesc;
    case 'v': *dst++ = '\v'; goto is_unesc;
    case 'e': *dst++ = '\x1B'; goto is_unesc;

    case '\0':

      *dst++ = '\\';
      *dst = chr;
      return dst - ini_string;

    default:

      *dst++ = '\\';
      *dst++ = chr;
      goto is_unesc;

  }

}

