/*  examples/topics/strip_ini_cache.c  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <confini.h>

static int my_callback (IniDispatch * dispatch, void * v_null) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  const char original_ini_buffer[] = 
    "[SectionOne]\n"
    "\n"
    "key = \"value\"\n"
    "integer = 1234\n"
    "real = 3.14\n"
    "string1 = \"Case 1\"\n"
    "string2 = 'Case 2'\n"
  ;

  size_t ini_length = strlen(original_ini_buffer);
  char * const ini_cache = strndup(original_ini_buffer, ini_length);

  if (!ini_cache || strip_ini_cache(
    ini_cache,
    ini_length,
    INI_DEFAULT_FORMAT,
    NULL,
    my_callback,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  printf(

    "The previous dispatches come from the following buffer:\n\n"
    "-------------[original INI buffer]-------------\n"
    "%s"
    "-----------------------------------------------\n\n",

    original_ini_buffer

  );

  printf(
    "After being processed by `strip_ini_cache()`, the buffer looks "
    "like this:\n"
  );

  printf("\n---------------[disposed buffer]---------------\n");

  for (size_t idx = 0; idx <= ini_length; idx++) {

    putchar(ini_cache[idx] == 0 ? '.' : ini_cache[idx]);

  }

  printf("\n-----------------------------------------------\n\n");

  printf(
    "The dots in the example above represent NUL characters. Remember "
    "that\n`strip_ini_cache()` does not free the buffer passed, you "
    "will have to do that\nby yourself.\n"
  );

  free(ini_cache);

  return 0;

}

