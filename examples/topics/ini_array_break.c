/*  examples/topics/ini_array_break.c  */

#include <stdio.h>
#include <confini.h>

static int my_ini_listener (IniDispatch * dispatch, void * v_null) {

  if (ini_string_match_si(
    "my_array",
    dispatch->data,
    dispatch->format
  )) {

    #define DELIMITER ','

    char * part_a, * part_b = dispatch->value;

    while ((part_a = part_b)) {

      part_b = ini_array_break(part_b, DELIMITER, dispatch->format);
      ini_string_parse(part_a, dispatch->format);
      printf("%s\n", part_a);

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

