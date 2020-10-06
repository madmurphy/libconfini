/*  examples/miscellanea/disambiguate.c  */
/*

See: Manual > Ini Syntax Considerations

*/

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * dispatch, void * v_null) {

  printf(
    "#%zu - Type: %u; Data: \"%s\"; Value: \"%s\"\n",
    dispatch->dispatch_id,
    dispatch->type,
    dispatch->data,
    dispatch->value
  );

  return 0;

}

int main () {

  #define MY_FORMAT \
    ((IniFormat) { \
      .delimiter_symbol = INI_EQUALS, \
      .case_sensitive = false, \
      .semicolon_marker = INI_DISABLED_OR_COMMENT, \
      .hash_marker = INI_ONLY_COMMENT, \
      .section_paths = INI_ABSOLUTE_AND_RELATIVE, \
      .multiline_nodes = INI_MULTILINE_EVERYWHERE, \
      .no_single_quotes = false, \
      .no_double_quotes = false, \
      .no_spaces_in_names = false, \
      .implicit_is_not_empty = true, \
      .do_not_collapse_values = false, \
      .preserve_empty_quotes = false, \
      .disabled_after_space = false, \
      .disabled_can_be_implicit = true \
    })

  printf(":: Content of \"ambiguous.conf\" ::\n\n");

  if (load_ini_path(
    "../ini_files/ambiguous.conf",
    MY_FORMAT,
    NULL,
    ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

