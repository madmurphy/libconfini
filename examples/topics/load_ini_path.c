/*  examples/topics/load_ini_path.c  */

#include <stdio.h>
#include <confini.h>

static int my_callback (IniDispatch * dispatch, void * v_null) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  if (load_ini_path(
    "../ini_files/delivery.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    my_callback,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

