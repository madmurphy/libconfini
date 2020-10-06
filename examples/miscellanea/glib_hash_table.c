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

In this implementation the dot is a metacharacter and is used as
delimiter between path parts. Therefore all dots appearing in key names,
or (within quotes) in section paths, will be replaced with the following
character. You may change it with any other character (except the dot itself).

*/
#define DOT_REPLACEMENT '-'

static inline void string_tolower (char * const str) {
  for (register char * chrptr = str; *chrptr; chrptr++) {
    *chrptr = *chrptr > 0x40 && *chrptr < 0x5b ? *chrptr | 0x60 : *chrptr;
  }
}

/*  Add each dispatch to the hash table (callback function)  */
static int populate_hash_table (IniDispatch * this, void * v_hash_table) {
  #define _MALLOC_CHECK_(PTR) \
    if (PTR == NULL) { fprintf(stderr, "malloc error\n"); return 1; }
  if (this->type != INI_KEY) {
    return 0;
  }
  size_t idx;
  char * ptr_a, * ptr_b, * ptr_c, * ptr_d;
  this->d_len = ini_unquote(this->data, this->format);
  /*  remove quoted dots from parent and allocate the first string  */
  if (this->at_len) {
    /*  has parent  */
    ptr_a = ptr_b = (char *) malloc(this->at_len + 1);
    _MALLOC_CHECK_(ptr_b)
    *((const char **) &ptr_d) = this->append_to;
    while ((ptr_c = ptr_d)) {
      idx = ini_array_shift((const char **) &ptr_d, '.', this->format);
      ptr_a[idx] = '\0';
      do {
        --idx;
        ptr_a[idx] = ptr_c[idx] == '.' ? DOT_REPLACEMENT : ptr_c[idx];
      } while (idx > 0);
      ptr_a += ini_unquote(ptr_a, this->format);
      *ptr_a++ = '.';
    }
    idx = ptr_a - ptr_b;
    ptr_a = (char *) malloc(idx + this->d_len + 1);
    _MALLOC_CHECK_(ptr_a)
    memcpy(ptr_a, ptr_b, idx);
    free(ptr_b);
    ptr_b = ptr_a + idx;
  } else {
    /*  parent is root  */
    ptr_a = ptr_b = (char *) malloc(this->d_len + 1);
    _MALLOC_CHECK_(ptr_a)
  }
  /*  remove dots from key name  */
  for (
    idx = this->d_len + 2;
      idx-- > 0;
    ptr_b[idx] = this->data[idx] == '.' ? DOT_REPLACEMENT : this->data[idx]
  );
  /*
    we are using `g_hash_table_lookup()` to search for the indicized keys,
    therefore it is better to convert the string to lower case -- but you
    may disagree
  */
  if (!this->format.case_sensitive) {
    string_tolower(ptr_a);
  }
  /*  check for duplicate keys  */
  if (g_hash_table_lookup_extended((GHashTable *) v_hash_table, ptr_a, (gpointer) &ptr_b, NULL)) {
    fprintf(stderr, "`%s` will be overwritten (duplicate key found)\n", ptr_a);
    if (!g_hash_table_remove((GHashTable *) v_hash_table, ptr_b)) {
      fprintf(stderr, "Unable to remove `%s` from the hash table\n", ptr_a);
    }
  }
  /*  allocate the second string  */
  ptr_b = (char *) malloc(this->v_len + 1);
  _MALLOC_CHECK_(ptr_b)
  if (this->value) {
    for (idx = this->v_len + 2; idx-- > 0; ptr_b[idx] = this->value[idx]);
  } else {
    *ptr_b = '\0';
  }
  g_hash_table_insert((GHashTable *) v_hash_table, ptr_a, ptr_b);
  printf("`%s` has been added to the hash table.\n", ptr_a);
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

