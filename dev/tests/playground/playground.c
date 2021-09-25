/*  dev/tests/extreme_ini/extreme_ini.c  */

#include <stdio.h>
#include <confini.h>


static int print_ini_stats (IniStatistics * stats, void * v_null) {

  printf(

    "The file is %zu bytes large and contains %zu members.\n"
    "Please find below the output of each dispatch.\n",

    stats->bytes,
    stats->members

  );

  return 0;

}


static int ini_listener (IniDispatch * dispatch, void * v_null) {

  printf(

    "\ndispatch_id: %zu\n"
    "format: {IniFormat}\n"
    "type: %u\n"
    "data: `%s`\n"
    "value%s: `%s`\n"
    "append_to: `%s`\n"
    "d_len: %zu\n"
    "v_len: %zu\n"
    "at_len: %zu\n",

    dispatch->dispatch_id,
    dispatch->type,
    dispatch->data,
    dispatch->value == INI_GLOBAL_IMPLICIT_VALUE ? " (implicit)" : "",
    dispatch->value,
    *dispatch->append_to ? dispatch->append_to : "{top level}",
    dispatch->d_len,
    dispatch->v_len,
    dispatch->at_len

  );

  return 0;

}


int main () {

  /*  Hackable format  */
  #define HACK_ME \
    ((IniFormat) { \
      .delimiter_symbol = INI_EQUALS, \
      .case_sensitive = false, \
      .semicolon_marker = INI_DISABLED_OR_COMMENT, \
      .hash_marker = INI_DISABLED_OR_COMMENT, \
      .section_paths = INI_ABSOLUTE_AND_RELATIVE, \
      .multiline_nodes = INI_MULTILINE_EVERYWHERE, \
      .no_single_quotes = false, \
      .no_double_quotes = false, \
      .no_spaces_in_names = false, \
      .implicit_is_not_empty = true, \
      .do_not_collapse_values = false, \
      .preserve_empty_quotes = false, \
      .disabled_after_space = false, \
      .disabled_can_be_implicit = false \
    })

  ini_global_set_implicit_value("YES", 3);

  if (
    load_ini_path(
      "extreme_ini.conf",
      HACK_ME,
      print_ini_stats,
      ini_listener,
      NULL
    )
  ) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

