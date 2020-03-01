/*  examples/topics/ini_string_parse.c  */

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * dispatch, void * v_null) {

  if (
    dispatch->type == INI_KEY || dispatch->type == INI_DISABLED_KEY
  ) {

    ini_unquote(dispatch->data, dispatch->format);
    ini_string_parse(dispatch->value, dispatch->format);

  }

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data,
    dispatch->value,
    dispatch->type
  );

  return 0;

}

int main () {

  if (load_ini_path(
    "../ini_files/self_explaining.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}
