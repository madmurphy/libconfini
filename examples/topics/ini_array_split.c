/*  examples/topics/ini_array_split.c  */

#include <stdio.h>
#include <confini.h>

static int my_array_memb_handler (
  char * const arr_member,
  const size_t memb_length,
  const size_t memb_num,
  const IniFormat format,
  void * const foreach_other
) {

  printf("\"%s\"\n", arr_member);

  return 0;

}

int main () {

  char my_ini_array[] = "first .   second   . third";

  ini_array_split(
    my_ini_array,
    '.',
    INI_DEFAULT_FORMAT,
    my_array_memb_handler,
    NULL
  );

  return 0;

}

