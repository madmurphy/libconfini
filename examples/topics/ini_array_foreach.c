/*  examples/topics/ini_array_foreach.c  */

#include <stdio.h>
#include <confini.h>

static int my_array_fragm_handler (
  const char * const ini_array,
  const size_t fragm_offset,
  const size_t fragm_length,
  const size_t fragm_num,
  const IniFormat format,
  void * const user_data
) {

  printf("\"%.*s\"\n", (unsigned int) fragm_length, ini_array + fragm_offset);

  return 0;

}

int main () {

  ini_array_foreach(
    "first  ,  second   ,  third",
    ',',
    INI_DEFAULT_FORMAT,
    my_array_fragm_handler,
    NULL
  );

  return 0;

}

