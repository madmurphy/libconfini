/*  examples/miscellanea/typed_ini.c  */
/*

The following code will try to read an INI section called `my_section`,
expected to contain the following typed data:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

[my_section]

my_string = [string]
my_number = [number]
my_boolean = [boolean]
my_implicit_boolean
my_array = [comma-delimited array]

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No errors will be generated if any of the data above are absent.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <confini.h>

#include "../utilities/make_strarray.h"

#define MY_ARRAY_DELIMITER ','

/*  My stored data  */
struct ini_store {
  char * my_section_my_string;
  int my_section_my_number;
  bool my_section_my_boolean;
  bool my_section_my_implicit_bool;
  char ** my_section_my_array;
  size_t my_section_my_arr_len;
};

static int my_init (IniStatistics * statistics, void * v_store) {
  *((struct ini_store *) v_store) = (struct ini_store) {
    .my_section_my_string = NULL,
    .my_section_my_number = -1,
    .my_section_my_boolean = false,
    .my_section_my_implicit_bool = false,
    .my_section_my_array = NULL,
    .my_section_my_arr_len = 0
  };
  return 0;
}

static int my_handler (IniDispatch * dsp, void * v_store) {
  #define store ((struct ini_store *) v_store)
  #define THEYMATCH(SSTR, ISTR) \
    ini_string_match_si(SSTR, ISTR, dsp->format)
  if (dsp->type == INI_KEY && THEYMATCH("my_section", dsp->append_to)) {
    if (THEYMATCH("my_string", dsp->data)) {
      dsp->v_len = ini_string_parse(dsp->value, dsp->format);
      /*  Free previous duplicate key (if any)  */
      if (store->my_section_my_string) {
        free(store->my_section_my_string);
      }
      /*  Allocate the new string  */
      store->my_section_my_string = strndup(dsp->value, dsp->v_len);
      if (!store->my_section_my_string) {
        return 1;
      }
    } else if (THEYMATCH("my_number", dsp->data)) {
      store->my_section_my_number = ini_get_int(dsp->value);
    } else if (THEYMATCH("my_boolean", dsp->data)) {
      store->my_section_my_boolean = ini_get_bool_i(
        dsp->value,
        0,
        dsp->format
      );
    } else if (THEYMATCH("my_implicit_boolean", dsp->data)) {
      store->my_section_my_implicit_bool = ini_get_bool_i(
        dsp->value,
        1,
        dsp->format
      );
    } else if (THEYMATCH("my_array", dsp->data)) {
      /*  Save memory (not strictly needed)  */
      dsp->v_len = ini_array_collapse(
        dsp->value,
        MY_ARRAY_DELIMITER,
        dsp->format
      );
      /*  Free previous duplicate key (if any)  */
      if (store->my_section_my_array) {
        free(store->my_section_my_array);
      }
      /*  Allocate a new array of strings  */
      /*  Function in examples/utilities/make_strarray.h  */
      store->my_section_my_array = make_strarray(
        &store->my_section_my_arr_len,
        dsp->value,
        dsp->v_len,
        MY_ARRAY_DELIMITER,
        dsp->format
      );
      if (!store->my_section_my_array) {
        return 1;
      }
    }
  }
  return 0;
  #undef THEYMATCH
  #undef store
}

static void print_stored_data (const struct ini_store * const store) {
  printf(
    "my_string -> %s\n"
    "my_number -> %d\n"
    "my_boolean -> %s\n"
    "my_implicit_boolean -> %s\n"
    "my_array[%zu] -> [%s",
    store->my_section_my_string,
    store->my_section_my_number,
    store->my_section_my_boolean ? "True (`1`)" : "False (`0`)",
    store->my_section_my_implicit_bool ? "True (`1`)" : "False (`0`)",
    store->my_section_my_arr_len,
    store->my_section_my_arr_len ? store->my_section_my_array[0] : ""
  );
  for (size_t idx = 1; idx < store->my_section_my_arr_len; idx++) {
    printf("|%s", store->my_section_my_array[idx]);
  }
  printf("]\n");
}

int main () {
  IniFormat my_format;
  struct ini_store my_store;
  ini_global_set_implicit_value("YES", 3);
  my_format = INI_DEFAULT_FORMAT;
  my_format.implicit_is_not_empty = true;
  my_format.disabled_can_be_implicit = true;
  my_format.semicolon_marker = my_format.hash_marker = INI_IGNORE;
  if (load_ini_path(
    "../ini_files/typed_ini.conf",
    my_format,
    my_init,
    my_handler,
    &my_store
  )) {
    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;
  }
  print_stored_data(&my_store);
  if (my_store.my_section_my_string) {
    free(my_store.my_section_my_string);
  }
  if (my_store.my_section_my_arr_len) {
    free(my_store.my_section_my_array);
  }
  return 0;
}

