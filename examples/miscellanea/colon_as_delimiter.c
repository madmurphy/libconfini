/*  examples/miscellanea/colon_as_delimiter.c  */

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * dispatch, void * v_null) {

  printf(
    "DATA: %s\nVALUE: %s\nPARENT SECTION: %s\n\n",
    dispatch->data,
    dispatch->value,
    *dispatch->append_to ? dispatch->append_to : "{root}"
  );

  return 0;

}

int main () {

  #define OFF 0
  #define ON 1

  #define MY_FORMAT \
    ((IniFormat) { \
      .delimiter_symbol = INI_COLON,    /*  or ':'  */ \
      .case_sensitive = OFF, \
      .semicolon_marker = INI_IGNORE, \
      .hash_marker = INI_IGNORE, \
      .section_paths = INI_ABSOLUTE_AND_RELATIVE, \
      .multiline_nodes = INI_MULTILINE_EVERYWHERE, \
      .no_single_quotes = OFF, \
      .no_double_quotes = OFF, \
      .no_spaces_in_names = OFF, \
      .implicit_is_not_empty = OFF, \
      .do_not_collapse_values = OFF, \
      .preserve_empty_quotes = OFF, \
      .disabled_after_space = OFF, \
      .disabled_can_be_implicit = OFF \
    })

  if (load_ini_path(
    "../ini_files/colon_as_delimiter.conf",
    MY_FORMAT,
    NULL,
    ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

  #undef OFF
  #undef ON

}

