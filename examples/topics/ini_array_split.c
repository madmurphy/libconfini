/*  examples/topics/ini_array_split.c  */

#include <stdio.h>
#include <confini.h>

static int my_array_memb_handler (
  char * arr_member,
  size_t memb_length,
  size_t memb_num,
  IniFormat format,
  void * foreach_other
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

