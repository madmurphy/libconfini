/*  examples/topics/ini_array_foreach.c  */

#include <stdio.h>
#include <confini.h>

static int my_array_fragm_handler (
  const char * ini_array,
  size_t fragm_offset,
  size_t fragm_length,
  size_t fragm_num,
  IniFormat format,
  void * user_data
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

