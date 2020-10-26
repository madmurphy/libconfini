/*  examples/miscellanea/dispatch_everything.c  */
/*

The following code will try to print on screen every information dispatched by
**libconfini** when an INI file is parsed.

*/

#include <stdio.h>
#include <confini.h>

#define FALSE 0
#define TRUE 1

struct my_struct {

  char * my_date;
  char * my_ini_file;

};

static int print_ini_stats (IniStatistics * stats, void * user_data) {

  struct my_struct * my_other = (struct my_struct *) user_data;

  printf(

    "Hi there!\n\n"
    "Today is %s.\n\n"
    "We decided to parse the INI file \"%s\".\n\n"
    "The file is %zu bytes large and contains %zu members.\n"
    "Please find below the output of each dispatch.\n",

    my_other->my_date,
    my_other->my_ini_file,
    stats->bytes,
    stats->members

  );

  return 0;

}

static int my_ini_listener (IniDispatch * dispatch, void * user_data) {

  printf(

    "\ndispatch_id: %zu\n"
    "format: {IniFormat}\n"
    "type: %u\n"
    "data: `%s`\n"
    "value: `%s`\n"
    "append_to: `%s`\n"
    "d_len: %zu\n"
    "v_len: %zu\n"
    "at_len: %zu\n",

    dispatch->dispatch_id,
    dispatch->type,
    dispatch->data,
    dispatch->value,
    *dispatch->append_to ? dispatch->append_to : "{root}",
    dispatch->d_len,
    dispatch->v_len,
    dispatch->at_len

  );

  /*  Check if this is an implicit key  */

  if (dispatch->value == INI_GLOBAL_IMPLICIT_VALUE) {

    printf("\n\tHey! This was an implicit value!\n");

  }

  return 0;

}

int main () {

  /*  The format of the INI file  */
  IniFormat my_format;

  /*  User data  */
  struct my_struct my_other;

  /*  Define the value to throw with implicit keys, and its length  */
  ini_global_set_implicit_value("YES", 3);

  my_other.my_date = "Thursday September 22th, 2016";
  my_other.my_ini_file = "../ini_files/self_explaining.conf";

  /*  Use the default format as model for the new format  */
  my_format = INI_DEFAULT_FORMAT;

  /*  Enable implicit keys for this format  */
  my_format.implicit_is_not_empty = true;
  my_format.disabled_can_be_implicit = true;

  /*  Load the INI file  */
  if (load_ini_path(
    my_other.my_ini_file,
    my_format,
    print_ini_stats,
    my_ini_listener,
    &my_other
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  printf(
    "\nFile \"%s\" has been parsed successfully.\n\n",
    my_other.my_ini_file
  );

  return 0;

}

