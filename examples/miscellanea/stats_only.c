/*  examples/miscellanea/stats_only.c  */
/*

The following code will try to load an INI file in order to get some statistics
about it, without dispatching its content.

*/

#include <stdio.h>
#include <confini.h>

static int my_stats_handler (IniStatistics * statistics, void * user_data) {

  printf(
    "The file is %zu bytes large and contains %zu members.\n",
    statistics->bytes,
    statistics->members
  );

  return 0;

}

int main () {

  if (load_ini_path(
    "../ini_files/self_explaining.conf",
    INI_DEFAULT_FORMAT,
    my_stats_handler,
    NULL,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

