/*  dev/tests/performance/performance.c  */

#include <stdio.h>
#include <confini.h>
#include <time.h>

static int get_ini_size (IniStatistics * stats, void * v_data) {

  (*(size_t (*)[2]) v_data)[0] = stats->bytes;
  (*(size_t (*)[2]) v_data)[1] = stats->members;
  return 0;

}

static int empty_listener (IniDispatch * dispatch, void * v_data) {

  return 0;

}

int main () {

  /*  Hackable clone of `INI_DEFAULT_FORMAT`  */
  #define PERFORMANCE_FORMAT \
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
      .implicit_is_not_empty = false, \
      .do_not_collapse_values = false, \
      .preserve_empty_quotes = false, \
      .disabled_after_space = false, \
      .disabled_can_be_implicit = false \
    })

  /*  Not used here, but try it!  */
  #define INI_FAST_FORMAT \
    ((IniFormat) { \
      .delimiter_symbol = INI_EQUALS, \
      .case_sensitive = false, \
      .semicolon_marker = INI_IGNORE, \
      .hash_marker = INI_IGNORE, \
      .section_paths = INI_ABSOLUTE_AND_RELATIVE, \
      .multiline_nodes = INI_NO_MULTILINE, \
      .no_single_quotes = false, \
      .no_double_quotes = false, \
      .no_spaces_in_names = false, \
      .implicit_is_not_empty = false, \
      .do_not_collapse_values = true, \
      .preserve_empty_quotes = true, \
      .disabled_after_space = false, \
      .disabled_can_be_implicit = false \
    })

  /*
    size_t stats[2] = (size_t [2]) {
      IniStatistics::bytes,
      IniStatistics::members
    }
  */
  size_t stats[2];
  double seconds;

  clock_t start, end;
  start = clock();

  /*  Please create an INI file large enough  */
  if (load_ini_path(
    "big_file.ini",
    PERFORMANCE_FORMAT,
    get_ini_size,
    empty_listener,
    &stats
  )) {

    return 1;

  }

  end = clock();
  seconds = (double) (end - start) / CLOCKS_PER_SEC;

  printf(
    "%zu bytes (%zu nodes) parsed in %f seconds.\n"
    "Number of bytes parsed per second: %f\n",
    stats[0], stats[1], seconds, (double) stats[0] / seconds
  );

  return 0;

}

