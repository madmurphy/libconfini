/*  dev/tests/null-byte_injected.c  */

#include <stdio.h>
#include <stdlib.h>
#include <confini.h>

static int my_ini_listener (IniDispatch *dispatch, void *user_data) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}


int main () {

  FILE * const ini_file = fopen("null-byte_injected.conf", "rb");

  if (ini_file == NULL) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  /*  We need another copy of the file in memory to print it on screen...  */

  fseek(ini_file, 0, SEEK_END);

  const size_t file_size = ftell(ini_file);
  char * const file_content = (char *) malloc(file_size + 1);

  if (!file_content) {

    fprintf(stderr, "Sorry, not enough memory :-(\n");
    return 1;

  }

  rewind(ini_file);

  if (fread(file_content, 1, file_size, ini_file) < file_size) {

    fprintf(stderr, "Sorry, an error occurred while reading the file :-(\n");
    return 1;

  }

  printf(
    "Actual content of ini_files/null-byte_injected.conf (the dots represent null\n"
    "characters):\n\n"
    "---------------------\n"
  );

  for (size_t idx = 0; idx < file_size; idx++) {

    putchar(file_content[idx] == 0 ? '.' : file_content[idx]);

  }

  printf("\n---------------------\n\nParsed content:\n\n");

  /*  We are ready to parse the INI file  */

  if (load_ini_file(ini_file, INI_DEFAULT_FORMAT, NULL, my_ini_listener, file_content)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  fclose(ini_file);
  free(file_content);

  return 0;

}

