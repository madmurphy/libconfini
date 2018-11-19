/*  examples/miscellanea/disambiguate.c  */
/*

See: Manual > Ini Syntax Considerations

*/

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * dispatch, void * v_null) {

  printf(
    "#%zu - TYPE: %u, DATA: \"%s\", VALUE: \"%s\"\n",
    dispatch->dispatch_id,
    dispatch->type,
    dispatch->data,
    dispatch->value
  );

  return 0;

}

int main () {

  #define NO 0
  #define YES 1

  IniFormat my_format = {
    .delimiter_symbol = INI_EQUALS,
    .case_sensitive = NO,
    .semicolon_marker = INI_DISABLED_OR_COMMENT,
    .hash_marker = INI_ONLY_COMMENT,
    .section_paths = INI_ABSOLUTE_AND_RELATIVE,
    .multiline_nodes = INI_MULTILINE_EVERYWHERE,
    .no_single_quotes = NO,
    .no_double_quotes = NO,
    .no_spaces_in_names = NO,
    .implicit_is_not_empty = YES,
    .do_not_collapse_values = NO,
    .preserve_empty_quotes = NO,
    .disabled_after_space = NO,
    .disabled_can_be_implicit = YES
  };

  #undef NO
  #undef YES

  printf(":: Content of \"ambiguous.conf\" ::\n\n");

  if (load_ini_path(
    "ini_files/ambiguous.conf",
    my_format,
    NULL,
    ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

