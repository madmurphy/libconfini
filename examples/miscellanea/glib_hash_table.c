/*  examples/miscellanea/glib_hash_table.c  */
/*

The following code will try to use GLib's `GHashTable` object to store the
entire content of an INI file. Each key will be accessible through its full
path -- e.g.: `[section.subsection.subsubsection...key`. Comments and disabled
entries will be ignored.

Furthermore, after the content has been stored, the `main()` function will
attempt to read the value of the key `my_section.my_subsection.find_me`.

*/


#include <stdio.h>
#include <confini.h>
#include <glib.h>
#include <stdbool.h>


/*

In this implementation the dot is a metacharacter and is used as delimiter
between path parts. Therefore all dots appearing in key names, or (within
quotes) in section paths, will be replaced with the following character. You
may change it with any other character (except the dot itself).

*/
#define DOT_REPLACEMENT '-'

static inline void string_tolower (char * const str) {
  for (register char * chrptr = str; *chrptr; chrptr++) {
    *chrptr = *chrptr > 0x40 && *chrptr < 0x5b ? *chrptr | 0x60 : *chrptr;
  }
}

/*  Add each dispatch to the hash table (callback function)  */
static int populate_hash_table (IniDispatch * disp, void * v_hash_table) {
  #define _MALLOC_CHECK_(PTR) \
    if (PTR == NULL) { fprintf(stderr, "malloc error\n"); return 1; }
  if (disp->type != INI_KEY) {
    return 0;
  }
  size_t idx;
  char * newptr1, * newptr2, * oldptr1, * oldptr2;
  disp->d_len = ini_unquote(disp->data, disp->format);
  /*  remove quoted dots from parent and allocate the first string  */
  if (disp->at_len) {
    /*  has parent  */
    newptr1 = newptr2 = (char *) malloc(disp->at_len + 1);
    _MALLOC_CHECK_(newptr2)
    *((const char **) &oldptr2) = disp->append_to;
    while ((oldptr1 = oldptr2)) {
      idx = ini_array_shift((const char **) &oldptr2, '.', disp->format);
      newptr1[idx] = '\0';
      while (idx > 0) {
        --idx;
        newptr1[idx] = oldptr1[idx] == '.' ? DOT_REPLACEMENT : oldptr1[idx];
      }
      newptr1 += ini_unquote(newptr1, disp->format);
      *newptr1++ = '.';
    }
    idx = newptr1 - newptr2;
    newptr1 = (char *) malloc(idx + disp->d_len + 1);
    _MALLOC_CHECK_(newptr1)
    memcpy(newptr1, newptr2, idx);
    free(newptr2);
    newptr2 = newptr1 + idx;
  } else {
    /*  parent is root  */
    newptr1 = newptr2 = (char *) malloc(disp->d_len + 1);
    _MALLOC_CHECK_(newptr1)
  }
  /*  remove dots from key name  */
  idx = disp->d_len + 1;
  do {
    --idx;
    newptr2[idx] = disp->data[idx] == '.' ? DOT_REPLACEMENT : disp->data[idx];
  } while (idx > 0);
  /*
    we are using `g_hash_table_lookup()` to search for the indicized keys,
    therefore it is better to convert the string to lower case -- but you
    may disagree
  */
  if (!disp->format.case_sensitive) {
    string_tolower(newptr1);
  }
  /*  check for duplicate keys  */
  if (g_hash_table_lookup_extended((GHashTable *) v_hash_table, newptr1, (gpointer) &newptr2, NULL)) {
    fprintf(stderr, "`%s` will be overwritten (duplicate key found)\n", newptr1);
    if (!g_hash_table_remove((GHashTable *) v_hash_table, newptr2)) {
      fprintf(stderr, "Unable to remove `%s` from the hash table\n", newptr1);
    }
  }
  /*  allocate the second string  */
  newptr2 = (char *) malloc(disp->v_len + 1);
  _MALLOC_CHECK_(newptr2)
  if (disp->value) {
    memcpy(newptr2, disp->value, disp->v_len + 1);
  } else {
    *newptr2 = '\0';
  }
  g_hash_table_insert((GHashTable *) v_hash_table, newptr1, newptr2);
  printf("`%s` has been added to the hash table.\n", newptr1);
  return 0;
  #undef _MALLOC_CHECK_
}


int main () {
  #define my_format \
    ((IniFormat) { \
      .delimiter_symbol = INI_EQUALS, \
      .case_sensitive = false, \
      .semicolon_marker = INI_IGNORE, \
      .hash_marker = INI_IGNORE, \
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
  GHashTable * my_hash_table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
  ini_global_set_implicit_value("true", 4);
  if (load_ini_path("../ini_files/hash_table.conf", my_format, NULL, populate_hash_table, my_hash_table)) {
    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;
  }
  /*
    Let's have a look at the value of `my_section.my_subsection.find_me`...
  */
  char
    * const my_key = "my_section.my_subsection.find_me",
    * const my_value = (char *) g_hash_table_lookup(my_hash_table, my_key);
  if (my_value) {
    /*
      We call `ini_string_parse()` on the value **after** this has been stored
      in the hash table. In this example we parse only a simple string, so this
      will not make any difference, but there might cases where a value can be
      an array, and in this case the unquoting should be done individually on
      each member. Using a hash table spares us the `if`/`else` chain when
      deciding if and how to unquote things.
    */
    ini_string_parse(my_value, my_format);
    printf("\nKey `%s` has value `%s`.\n", my_key, my_value);
  } else {
    printf("\nKey `%s` has not been found.\n", my_key);
  }
  /*  Destroy the hash table  */
  g_hash_table_destroy(my_hash_table);
  return 0;
  #undef my_format
}

