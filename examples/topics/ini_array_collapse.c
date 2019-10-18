/*  examples/topics/ini_array_collapse.c  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <confini.h>

static int populate_strarray (
  char * part,
  size_t part_len,
  size_t idx,
  IniFormat format,
  void * v_array
) {

  ini_string_parse(part, format);
  ((char **) v_array)[idx] = part;

  return 0;

}

static int my_ini_listener (IniDispatch * dispatch, void * v_null) {

  if (ini_string_match_si(
    "my_array",
    dispatch->data,
    dispatch->format
  )) {

    #define DELIMITER ','

    char ** my_array;
    size_t my_array_length;

    /*  Save memory with `ini_array_collapse()`  */
    dispatch->v_len = ini_array_collapse(
                        dispatch->value,
                        DELIMITER,
                        dispatch->format
                      );

    /*  Allocate a new array of strings with `malloc()`  */
    my_array_length = ini_array_get_length(
                        dispatch->value,
                        DELIMITER,
                        dispatch->format
                      );

    my_array = (char **) malloc(my_array_length * sizeof(char *) +
                 dispatch->v_len + 1);

    /*  Copy the strings with `memcpy()`  */
    memcpy(
      my_array + my_array_length,
      dispatch->value,
      dispatch->v_len + 1
    );

    /*  Populate the array  */
    ini_array_split(
      (char *) (my_array + my_array_length),
      DELIMITER,
      dispatch->format,
      populate_strarray,
      my_array
    );

    #undef DELIMITER

    /*  Do something with `my_array`  */
    printf("Array `my_array` has been created.\n\n");

    for (size_t idx = 0; idx < my_array_length; idx++) {

      printf("my_array[%zu] -> %s\n", idx, my_array[idx]);

    }

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

