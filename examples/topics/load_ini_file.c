/*  examples/topics/load_ini_file.c  */

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

  FILE * const ini_file = fopen("../ini_files/delivery.conf", "rb");

  if (ini_file == NULL) {

    fprintf(stderr, "File doesn't exist :-(\n");
    return 1;

  }

  if (load_ini_file(
    ini_file,
    INI_DEFAULT_FORMAT,
    NULL,
    my_callback,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  fclose(ini_file);

  return 0;

}

