/*  examples/miscellanea/parse_foreign.c  */

#include <stdio.h>
#include <confini.h>

static int my_callback (IniDispatch * dispatch, void * v_null) {

  if (
    dispatch->type != INI_COMMENT &&
    dispatch->type != INI_INLINE_COMMENT
  ) {

    printf(
      "DATA: %s\nVALUE: %s\nNODE TYPE: %s\nPARENT: %s\n\n",

      dispatch->data,

      dispatch->value == INI_GLOBAL_IMPLICIT_VALUE ?
        "true (implicit boolean)"
      :
        dispatch->value,

      dispatch->type == INI_SECTION ?
        "section"
      : dispatch->type == INI_KEY ?
        "key"
      : dispatch->type == INI_DISABLED_SECTION ?
        "disabled section"
      : dispatch->type == INI_DISABLED_KEY ?
        "disabled key"
      :
        "unknown",

      dispatch->at_len ?
        dispatch->append_to
      :
        "{top-level}"
    );

  }

  return 0;

}

int main () {

  ini_global_set_implicit_value("YES", 3);

  printf(
    "We are going to parse `ini_files/pacman.conf`, which is a"
    " clone of\n`/etc/pacman.conf` from Arch GNU/Linux (pacman"
    " version 5.2.1)...\n\n(ignoring comments)\n\n"
  );

  /*
    Original parser:
    https://git.archlinux.org/pacman.git/plain/src/common/ini.c
  */
  #define PACMAN_CONF_FORMAT \
    ((IniFormat) { \
      .delimiter_symbol = INI_EQUALS, \
      .case_sensitive = true, \
      .semicolon_marker = INI_IS_NOT_A_MARKER, \
      .hash_marker = INI_DISABLED_OR_COMMENT, \
      .multiline_nodes = INI_NO_MULTILINE, \
      .section_paths = INI_ABSOLUTE_ONLY, \
      .no_single_quotes = true, \
      .no_double_quotes = true, \
      .no_spaces_in_names = false, \
      .implicit_is_not_empty = true, \
      .do_not_collapse_values = false, \
      .preserve_empty_quotes = false, \
      .disabled_after_space = false, \
      .disabled_can_be_implicit = true \
    })

  if (load_ini_path(
    "../ini_files/pacman.conf",
    PACMAN_CONF_FORMAT,
    NULL,
    my_callback,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

