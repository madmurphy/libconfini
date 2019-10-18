/*  examples/topics/ini_array_shift.c  */

#include <stdio.h>
#include <confini.h>

static int my_ini_listener (IniDispatch * dispatch, void * v_null) {

  if (ini_string_match_si(
    "my_array",
    dispatch->data,
    dispatch->format
  )) {

    #define DELIMITER ','

    size_t length;
    char * left_behind, * shifted = dispatch->value;

    while ((left_behind = shifted)) {

      length = ini_array_shift(
                 (const char **) &shifted,
                 DELIMITER,
                 dispatch->format
               );

      printf("%.*s\n", (unsigned int) length, left_behind);

    }

    #undef DELIMITER

  }

  return 0;

}

int main () {

  if (load_ini_path(
    "../ini_files/typed_ini.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    my_ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

