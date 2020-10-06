/*  examples/topics/ini_global_set_implicit_value.c  */

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * dispatch, void * v_null) {

  if (dispatch->value == INI_GLOBAL_IMPLICIT_VALUE) {

    printf(
      "\nDATA: %s\nVALUE: %s\nNODE TYPE: %u\n"
      "(This is an implicit key element)\n",

      dispatch->data,
      dispatch->value,
      dispatch->type
    );

  } else {

    printf(
      "\nDATA: %s\nVALUE: %s\nNODE TYPE: %u\n",

      dispatch->data,
      dispatch->value,
      dispatch->type
    );

  }

  return 0;

}

int main () {

  IniFormat my_format = INI_UNIXLIKE_FORMAT;

  ini_global_set_implicit_value("[implicit default value]", 24);

  /*  Without this implicit keys will be considered empty  */
  my_format.implicit_is_not_empty = true;
  my_format.disabled_can_be_implicit = true;

  if (load_ini_path(
    "../ini_files/unix-like.conf",
    my_format,
    NULL,
    ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

}

