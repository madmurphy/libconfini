/*  examples/miscellanea/glib_hash_table.c  */
/*

The following code will try to use GLib's `GHashTable` object to store
the entire content of an INI file. Each key will be accessible through its full
path -- e.g.: `[section.[subsection.[subsubsection.]]]key`. Comments and
disabled entries will be ignored.

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
character. You may choose any character except the dot itself.

*/
#define DOT_REPLACEMENT '_'

/*  Parse and sanitize each part of a section path  */

static int path_part_handler (
  const char * unparsed_path,
  size_t offset,
  size_t len,
  size_t partnum,
  IniFormat ini_format,
  void * v_destp
) {

  char * dest = *((char **) v_destp);

  for (size_t idx = offset; idx < len + offset; idx++) {

    *dest++ = unparsed_path[idx] == '.' ? DOT_REPLACEMENT : unparsed_path[idx];

  }

  *dest = '\0';
  dest += ini_unquote(dest - len, ini_format) - len;
  *dest++ = '.';
  *((char **) v_destp) = dest;

  return 0;

}

/*  Push each dispatch to the hash table  */
static int populate_hash_table (
  IniDispatch * this,
  void * v_hash_table
) {

  #define _MALLOC_CHECK_(PTR) \
    if (PTR == NULL) { fprintf(stderr, "malloc error\n"); return 1; }

  if (this->type != INI_KEY) {

    return 0;

  }

  size_t tmp_size;
  char * str_a, * str_b;

  if (!INIFORMAT_HAS_NO_ESC(this->format)) {

    /*

      We might have quotes to remove or escape sequences to unescape

    */

    this->d_len = ini_unquote(this->data, this->format);

    if (this->value != INI_GLOBAL_IMPLICIT_VALUE) {

      this->v_len = ini_string_parse(this->value, this->format);

    }

  }

  if (this->at_len) {

    /*  We are temporary borrowing `tmp_size`, `str_a` and `str_b` to parse the section path  */
    #define _PATH_LEN_ tmp_size
    #define _END_PTR_ str_a
    #define _PARSED_PATH_ str_b

    /*  We cannot edit the buffer `dispatch->append_to`. So let's copy it.  */
    _END_PTR_ = _PARSED_PATH_ = (char *) malloc(this->at_len + 2);
    _MALLOC_CHECK_(_PARSED_PATH_)
    ini_array_foreach(this->append_to, '.', this->format, path_part_handler, &_END_PTR_);
    *_END_PTR_ = '\0';
    _PATH_LEN_ = _END_PTR_ - _PARSED_PATH_;

    /*  We can *finally* leave `str_a` available for pointing to the full path  */
    #undef _END_PTR_
    str_a = (char *) malloc(_PATH_LEN_ + this->d_len + 1);
    _MALLOC_CHECK_(str_a)
    memcpy(str_a, _PARSED_PATH_, _PATH_LEN_);
    free(_PARSED_PATH_);
    #undef _PARSED_PATH_
    /*  `str_b` is now available  */

    /*  For the `data` part we need to borrow again `str_b`  */
    str_b = str_a + _PATH_LEN_;
    #undef _PATH_LEN_
    /*  `tmp_size` is now available  */

  } else {

    /*  For the `data` part we need to borrow `str_b`  */
    str_a = str_b = (char *) malloc(this->d_len + 1);
    _MALLOC_CHECK_(str_a)

  }

  #define _DATA_PART_ str_b

  for (
    tmp_size = this->d_len + 2;
      tmp_size-- > 0;
    _DATA_PART_[tmp_size] = this->data[tmp_size] == '.' ? DOT_REPLACEMENT : this->data[tmp_size]
  );

  #undef _DATA_PART_
  /*  `str_b` is now available  */

  /*  We are borrowing again `str_b` in order to search for duplicate keys  */
  #define _EXISTING_ str_b

  if (g_hash_table_lookup_extended((GHashTable *) v_hash_table, str_a, (gpointer) &_EXISTING_, NULL)) {

    fprintf(stderr, "`%s` will be overwritten (duplicate key found)\n", str_a);

    if (!g_hash_table_remove((GHashTable *) v_hash_table, _EXISTING_)) {

      fprintf(stderr, "Unable to remove `%s` from the hash table\n", str_a);

    }

  }

  #undef _EXISTING_
  /*  `str_b` is now available  */

  /*  We can *finally* leave `str_b` available for pointing to the value part  */
  str_b = (char *) malloc(this->v_len + 1);
  _MALLOC_CHECK_(str_b)

  for (tmp_size = this->v_len + 2; tmp_size-- > 0; str_b[tmp_size] = this->value[tmp_size]);

  g_hash_table_insert((GHashTable *) v_hash_table, str_a, str_b);
  printf("`%s` has been added to the hash table\n", str_a);

  #undef _MALLOC_CHECK_

  return 0;
}

int main () {

  /*  Important! We are using `g_hash_table_lookup()` to search for the indicized keys, therefore we must fold the character case */
  INI_GLOBAL_LOWERCASE_MODE = true;

  /*  Important! In this context the length given to implicit values *must* always represent their real length!  */
  ini_global_set_implicit_value("YES", 3);

  GHashTable * my_hash_table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
  IniFormat my_format = INI_DEFAULT_FORMAT;

  /*  Comments and disabled keys will be anyway ignored, so let's avoid that they be dispatched at all  */
  my_format.semicolon_marker = my_format.hash_marker = INI_IGNORE;
  /*  All other parameters of the INI format may be set freely  */

  if (load_ini_path("../ini_files/hash_table.conf", my_format, NULL, populate_hash_table, my_hash_table)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  /*  Let's have a look at the value of `my_section.my_subsection.find_me`...  */

  const char
    * const my_key = "my_section.my_subsection.find_me",
    * const my_value = (char *) g_hash_table_lookup(my_hash_table, my_key);

  if (my_value) {

    printf("\nKey `%s` has value `%s`.\n", my_key, my_value);

  } else {

    printf("\nKey `%s` has not been found.\n", my_key);

  }

  /*  Destroy the hash table  */

  g_hash_table_destroy(my_hash_table);

  return 0;

}

