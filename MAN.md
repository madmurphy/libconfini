Library Functions Manual {#libconfini}
======================================

## DESCRIPTION

**libconfini** is a simple INI parsing library with the ability to read disabled entries (i.e., valid entries nested in comments). **libconfini** does not store the data read from an INI file, but rather dispatches it, formatted, to a custom listener.

The code is written in C and does not depend on any particular library, except for the C standard libraries **stdio.h**, **stdlib.h** and **stdint.h**.

If you want to start to learn directly from the code, you can find partially self-documented sample usages of **libconfini** under `/usr/share/doc/libconfini/examples/`.

## WHAT IS AN INI FILE

INI files were introduced with the early versions of Microsoft Windows, where the .ini file name extension stood for INItialization. An INI file can be considered as a string representation of a tree object, with new lines used as delimiters between nodes. A typical INI file is a plain text file looking like the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

# delivery.conf

; general options

location = Colosseum
place = Rome

[sender]

name = John Smith
email = john.smith@example.com

[receiver]

name = Mario Rossi   # He's a fat guy
email = mario.rossi@example.com

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## SUPPORTED SYNTAXES

During the years, several interpretations of INI files appeared. In some implementations the colon character (`:`) has been adopted as delimiter between keys and values (a typical example under GNU/Linux is `/etc/nsswitch.conf`), in other implementations the space (`/[ \t\v\f]+/` or `/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`) has been used instead (see for example `/etc/host.conf`), and so on. This library has been born as a general INI parser for GNU, so the support of most of the main INI dialects has been implemented within it.

Especially in Microsoft Windows a more radical syntax variation has been implemented: the use of the semicolon, instead of new lines, as delimiter between nodes, as in the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

# delivery.conf

; general options

location = Colosseum; place = Rome; [sender] name = John Smith;
email = john.smith@example.com; [receiver] name = Mario Rossi; # He's a fat guy
email = mario.rossi@example.com

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For several reasons the use of semicolon as node delimiter is not (and will never be) supported by **libconfini**.

### KEYS

A **key element** is identified as a string followed by a delimiter -- typically the equals sign (`=`) or the colon sign (`:`) or a space sequence (`/\s+/`) -- which is followed by a value, which is followed by a new line or an inline comment. 

Both the **key part** and the **value part** may be enclosed within quotes (`'` or `"`):

~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

foo = 'bar'
"hello" = world
"artist" = "Pablo Picasso"

~~~~~~~~~~~~~~~~~~~~~~~~~~

The **key part** can contain any character, except the delimiter (which may be enclosed within quotes for not beeing considered as such). In multiline formats internal new line sequences must be escaped (`/\\(?:\n\r?|\r\n?)/`).

If the **key part** part is missing the element is considered of unknown type, i.e., `INI_UNKNOWN` -- see `enum` `#IniNodeType` -- (example: `= foo`). If the **value part** is missing the key element is considered empty (example: `foo =`). If the delimiter is missing, according to some formats the key element is considered to be an _implicit key_ -- typically representing the boolean `TRUE` (example: `foo`). For instance, in the following example from `/etc/pacman.conf`, `IgnorePkg` is an empty key, while `Color` is an implicit key (representing a `TRUE` boolean -- i.e., `Color = YES`):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

HoldPkg = pacman glibc
Architecture = auto
IgnorePkg =
Color
SigLevel = Required DatabaseOptional
LocalFileSigLevel = Optional

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The **value** part can contain typed data, usually: a boolean (booleans supported by **libconfini** are: `NO`/`YES`, `FALSE`/`TRUE`, `0`/`1`), a string, a number, or an array (typically with commas as delimiters between members -- example: `paths = /etc, /usr, "/home/john/Personal Data"`). In multiline formats internal new line sequences must be escaped (`/\\(?:\n\r?|\r\n?)/`).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

[my_section]

my_string = "Hello world"
'my_number' = 42
my_boolean = NO
my_implicit_boolean
my_array = Asia, Africa, 'North America', South America,\
           Antarctica, Europe, Australia

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### SECTIONS

A **section** may be imagined like a directory. A **section path** is identified as the string `"$1"` in the regular expression `/(?:^|\n)[ \t\v\f]*\[[ \t\v\f]*([^\]]*)[ \t\v\f]*\]/` globally applied to an INI file. A section path expresses nesting through the 'dot' character, as in the following example:

~~~~~~~~~~~~~~~~~~~~{.ini}

[section]

foo = bar

[section.subsection]

foo = bar

~~~~~~~~~~~~~~~~~~~~

A section path starting with a dot expresses nesting to the previous section. Hence the last example is equivalent to:

~~~~~~~~~~~~~{.ini}

[section]

foo = bar

[.subsection]

foo = bar

~~~~~~~~~~~~~

Keys appearing before any section path belong to a virtual _root_ node (with an empty string as path), as the key `foo` in the following example:

~~~~~~~~~~~~~~~~~~~{.ini}

foo = bar

[options]

interval = 3600

[host]

address = 127.0.0.1
port = 80

~~~~~~~~~~~~~~~~~~~

Section parts may be enclosed within quotes:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

["world".europe.'germany'.berlin]

foo = bar

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### COMMENTS

Comments are string segments enclosed within the sequence `/(?:^|\s)[;#]/` and a new line sequence, as in the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

# this is a comment

foo = bar       # this is an inline comment

; this is another comment

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Comments may in theory be multiline, following the same syntax of multiline disabled entries (see below). This is usually of little utility, except for inline comments that you want to make sure will refer to the previous entry:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

comedy1 = The Tempest

comedy2 = Twelfth Night  # If music be the food of love, play on;      \
                         # Give me excess of it; that, surfeiting,     \
                         # The appetite may sicken, and so die.        \
                         # That strain again; it had a dying fall:     \
                         # O, it came oer my ear, like the sweet sound \
                         # That breathes upon a bank of violets,       \
                         # Stealing, and giving odour! Enough! No more.\
                         # 'Tis not so sweet now as it was before.     \
                         #                                             \
                         #     Orsino, scene I

# This is also a masterpiece!
comedy3 = The Merchant of Venice

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### DISABLED ENTRIES

A disabled entry is either a section or a key that has been nested inside a comment as its only child. Inline comments cannot represent disabled entries. Disabled entries can be multiline, using `/\\(?:\n\r?|\r\n?)[\t \v\f]*[;#]+/` as multiline escaping sequence. For example:

~~~~~~~~~~~~~~~{.ini}

#this = is\
 #a\
    #multiline\
#disabled\
  #entry

~~~~~~~~~~~~~~~

### ESCAPING SEQUENCES

In order to maximize the flexibility of the data, only four escaping sequences are supported by **libconfini**: `\'`, `\"`, `\\` and the multiline escaping sequence (`/\\(?:\n\r?|\r\n?)/`).

The first three escaping sequences are left untouched by all functions except `ini_unquote()`. Nevertheless, the characters `'`, `"` and `\` can determine different behaviors during the parsing depending on whether they are escaped or unescaped. For instance, the string `johnsmith !"` in the following example will not be parsed as a comment:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

[users.jsmith]

comment = "hey! have a look at my hashtag #johnsmith !"

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A particular case of escaping sequence is the multiline escaping sequence (`/\\(?:\n\r?|\r\n?)/`), which in multiline INI files gets _immediately unescaped by **libconfini**_.

~~~~~~~~~~~{.ini}

foo = this\
is\
a\
multiline\
value

~~~~~~~~~~~

## READ AN INI FILE

The syntax of **libconfini**'s parsing functions is:

\#1

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
int load_ini_file (
	FILE * ini_file,
	IniFormat format,
	int (*f_init)(
		IniStatistics *statistics,
		void *user_data
	),
	int (*f_foreach)(
		IniDispatch *dispatch,
		void *user_data
	),
	void *user_data
)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

\#2

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
int load_ini_path (
	char *path,
	IniFormat format,
	int (*f_init)(
		IniStatistics *statistics,
		void *user_data
	),
	int (*f_foreach)(
		IniDispatch *dispatch,
		void *user_data
	),
	void *user_data
)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

where

* `ini_file` in `load_ini_file()` is the `FILE` struct pointing to the INI file
* `path` in `load_ini_path()` is the path where the INI file is located (pointer to a char array, a.k.a. a "C string")
* `format` is a bitfield structure defining the syntax of the INI file (see the `IniFormat` struct)
* `f_init` is the function that will be invoked _before_ any dispatching begins -- it can be `NULL`
* `f_foreach` is the callback function that will be invoked for each member of the INI file - it can be `NULL`
* `user_data` is a pointer to a custom argument -- it can be `NULL`

The function `f_init()` is invoked with two arguments:

* `statistics` -- a pointer to an `IniStatistics` object containing some properties about the file read
  (like its size in bytes and the number of its members)
* `user_data` -- a pointer to the custom argument previously passed to the `load_ini_file()` / `load_ini_path()` functions

The function `f_foreach()` is invoked with two arguments:

* `dispatch` -- a pointer to an `IniDispatch` object containing the parsed member of the INI file
* `user_data` -- a pointer to the custom argument previously passed to the `load_ini_file()` / `load_ini_path()` functions

Both functions `load_ini_file()` and `load_ini_path()` return zero if the INI file has been completely parsed, non-zero otherwise.

## BASIC EXAMPLES

\#1

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *user_data) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %d\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  if (load_ini_path("my_file.conf", INI_DEFAULT_FORMAT, NULL, ini_listener, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

\#2

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *user_data) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %d\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  FILE * const ini_file = fopen("my_file.conf", "r");

  if (ini_file == NULL) {

    fprintf(stderr, "File doesn't exist :-(\n");
    return 1;

  }

  if (load_ini_file(ini_file, INI_DEFAULT_FORMAT, NULL, ini_listener, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  fclose(ini_file);

  return 0;

}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## HOW IT WORKS

The function `load_ini_path()` is a shortcut to the function `load_ini_file()` using a path instead of a `FILE` struct.

The function `load_ini_file()` dynamically allocates at once the whole INI file into the heap, and the two structures `IniStatistics` and `IniDispatch` into the stack. All the members of the INI file are then dispatched to the listener `f_foreach()`. Finally the allocated memory gets automatically freed.

Because of this mechanism _it is very important that all the dispatched data be **immediately** copied by the user (when needed), and no pointers to the passed data be saved_: after the end of the functions `load_ini_file()` / `load_ini_path()` all the allocated data will be destroyed indeed.

Within a dispatching cycle, the structure containing each dispatch (`IniDispatch *dispatch`) is always the same `struct` that gets constantly updated with new information.

The strings passed with each dispatch, as already said, must not be freed. _Nevertheless, before being copied or analyzed they can be edited, **with some precautions**_:

1. Be sure that your edit remains within the buffer lengths given (see: `IniDispatch::d_len` and `IniDispatch::v_len`).
2. If you want to edit the content of `IniDispatch::data` and this contains a section path, the `IniDispatch::append_to` properties of its children _may_ refer to the same buffer: if you edit it you can no more rely on its children's `IniDispatch::append_to` properties (you will not make any damage, the loop will continue just fine: so if you think you are going to never use the property `IniDispatch::append_to` just do it).
3. Regarding `IniDispatch::value`, the buffer will not be shared between dispatches, so feel free to edit it.
4. Regarding `IniDispatch::append_to`, this buffer is likely to be shared with other dispatches: again, you will not destroy the world nor generate errors, but you will make the next `IniDispatch::append_to`s useless. Therefore, **the property `IniDispatch::append_to` should be considered read-only** -- this is just a logical imposition (and this is why `IniDispatch::append_to` is not passed as `const`).

Typical peaceful edits are the calls of the functions `ini_collapse_array()` and `ini_unquote()` directly on the buffer `IniDispatch::value` (but make sure that you are not going to edit the global string `#INI_IMPLICIT_VALUE`):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *user_data) {

  if (dispatch->type == INI_KEY || dispatch->type == INI_DISABLED_KEY) {

    ini_unquote(dispatch->value, dispatch->format);

  }

  printf("DATA: %s\nVALUE: %s\n", dispatch->data, dispatch->value);

  return 0;

}

int main () {

  if (load_ini_path("my_file.ini", INI_DEFAULT_FORMAT, NULL, ini_listener, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to set the value to be assigned to implicit keys, please use the `ini_set_implicit_value()` function. A _zero-length `TRUE`-boolean_ is usually a good choice:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

/* void ini_set_implicit_value (char * implicit_value, unsigned long int implicit_v_len); */

ini_set_implicit_value("YES", 0);

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alternatively, instead of `ini_set_implicit_value()` you can manually define at the beginning of your code the two global variables `#INI_IMPLICIT_VALUE` and `#INI_IMPLICIT_V_LEN`, which will be retrieved by **libconfini**:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#include <confini.h>

char *INI_IMPLICIT_VALUE = "YES";
unsigned long int INI_IMPLICIT_V_LEN = 3;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If not defined elsewhere, these variables are respectively `NULL` and `0` by default.

After having set the value to be assigned to implicit key elements, it is possible to test whether a dispatched key is implicit or not by comparing the address of its `value` property with the global variable `#INI_IMPLICIT_VALUE`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#include <stdio.h>
#include <confini.h>

#define NO 0
#define YES 1

int ini_listener (IniDispatch *dispatch, void *user_data) {

  if (dispatch->value == INI_IMPLICIT_VALUE) {

    printf(
      "\nDATA: %s\nVALUE: %s\n(This is an implicit key element)\n",
      dispatch->data, dispatch->value
    );

  } else {

    printf("\nDATA: %s\nVALUE: %s\n", dispatch->data, dispatch->value);

  }

  return 0;

}

int main () {

  IniFormat my_format;

  ini_set_implicit_value("[implicit default value]", 0);

  /* Without setting this, implicit keys will be anyway considered empty: */
  my_format.implicit_is_not_empty = YES;

  if (load_ini_path("my_file.conf", my_format, NULL, ini_listener, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## RENDERING

The output strings dispatched by **libconfini** will follow some formatting rules depending on their role within the INI file. First, the multiline sequences will be unescaped, then

* **Section paths** will be rendered according to ECMAScript `section_name.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/\s+/g, " ")` -- within single or double quotes, if active, the text will be rendered verbatim
* **Key names** will be rendered according to ECMAScript `key_name.replace(/^[\n\r]\s*|\s+/g, " ")` -- within single or double quotes, if active, the text will be rendered verbatim
* **Values**, if `format.do_not_collapse_values` is active, will be cleaned of spaces at the beginning and at the end, otherwise will be rendered though the same algorithm used for key names.
* **Comments**: if multiline, ECMAScript `comment_string.replace(/(^|\n\r?|\r\n?)[ \t\v\f]*[#;]+/g, "$1")`; otherwise, ECMAScript `comment_string.replace(/^[ \t\v\f]*[#;]+/, "")`.
* **Unknown nodes** will be rendered verbatim.

## SIZE OF THE DISPATCHED DATA

Within an INI file it is granted that if one sums together all the `(dispatch->d_len + 1)` and all the `(dispatch->v_len + 1)` > 1 received, the result will always be less-than or equal-to `statistics->bytes` (where `+ 1` represents the NUL terminators). **If one adds to this also all the `dispatch->at_len` properties, or if the `dispatch->v_len` properties of implicit keys are non-zero, the sum may exceed it.** This can be relevant or irrelevant depending on your code.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#include <stdio.h>
#include <confini.h>

struct size_check {
  unsigned long int bytes, buff_lengths;
};

int ini_init (IniStatistics *stats, void *other) {

  ((struct size_check *) other)->bytes = stats->bytes;
  ((struct size_check *) other)->buff_lengths = 0;
  return 0;

}

int ini_listener (IniDispatch *this, void *other) {

  ((struct size_check *) other)->buff_lengths += this->d_len + 1 + (this->v_len ? this->v_len + 1 : 0);
  return 0;

}

int main () {

  struct size_check check;

  if (load_ini_path("my_file.ini", INI_DEFAULT_FORMAT, ini_init, ini_listener, &check)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  printf(

    "The file is %d bytes large.\n\nThe sum of the lengths of all "
    "IniDispatch::data buffers plus the lengths of all non-empty "
    "IniDispatch::value buffers is %d.\n",

    check.bytes, check.buff_lengths

  );

  /* `INI_IMPLICIT_V_LEN` is 0 and is not even used, so this cannot happen: */

  if (check.buff_lengths > check.bytes) {

    fprintf(stderr, "The end is near!");
    return 1;

  }

  return 0;

}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## THE `IniFormat` BITFIELD

For a correct use of this library it is fundamental to understand the `IniFormat` bitfield. **libconfini** has been born as a general INI parser, with the main purpose of _being able to understand INI files written by other programs_ (see README), therefore some flexibility was required.

When an INI file is parsed it is parsed according to a format. The `IniFormat` bitfield is a description of such format.

Each format can be represented also as a univocal 24-bit unsigned integer. In order to convert an `IniFormat` to an unsigned integer and vice versa please see `ini_format_get_id()`, `ini_format_set_to_id()` and `#IniFormatId`.

### THE MODEL FORMAT

A model format named `INI_DEFAULT_FORMAT` is available.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

IniFormat my_format;

my_format = INI_DEFAULT_FORMAT;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The code above corresponds to:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#define NO 0
#define YES 1

IniFormat my_format;

my_format.delimiter_symbol = INI_EQUALS;
my_format.semicolon = INI_PARSE_COMMENT;
my_format.hash = INI_PARSE_COMMENT;
my_format.multiline_entries = INI_EVERYTHING_MULTILINE;
my_format.case_sensitive = NO;
my_format.no_spaces_in_names = NO;
my_format.no_single_quotes = NO;
my_format.no_double_quotes = NO;
my_format.implicit_is_not_empty = NO;
my_format.do_not_collapse_values = NO;
my_format.no_disabled_after_space = NO;
my_format.disabled_can_be_implicit = NO; 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## THE `IniStatistics` AND `IniDispatch` STRUCTURES

When the functions `load_ini_file()` / `load_ini_path()` read an INI file, they dispatches the file content to the `f_foreach()` listener. Before the dispatching begins some statistics about the parsed file can be dispatched to the `f_init()` listener (if this is non-`NULL`).

The information passed to `f_init()` is passed through an `IniStatistics` structure, while the information passed to `f_foreach()` is passed through an `IniDispatch` structure.

## FORMATTING THE VALUES

Once your listener starts to receive the parsed data you may want to parse and better format the `value` part of key elements. The following functions may be useful for this purpose:

* `ini_unquote()`
* `ini_array_get_length()`
* `ini_collapse_array()`
* `ini_array_foreach()`
* `ini_split_array()`
* `ini_get_bool()`
* `ini_get_lazy_bool()`

Together with the functions listed above the following links are available, in case you don't want to `#include <stdlib.h>` in your source:

* `ini_get_int()` = [`atoi()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi)
* `ini_get_lint()` = [`atol()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol)
* `ini_get_llint()` = [`atoll()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll)
* `ini_get_float()` = [`atof()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof)

## FORMATTING THE KEY NAMES

The function `ini_unquote()` may be useful for key names enclosed within quotes.

## FORMATTING THE SECTION PATHS

In order to retrieve the parts of a section path the functions `ini_array_get_length()`, `ini_array_foreach()` and `ini_split_array()` can be used with '.' as delimiter. Note that section paths dispatched by **libconfini** are _always_ collapsed arrays, therefore calling the function `ini_collapse_array()` on them will have no effects.

It may be required that the function `ini_unquote()` be applied to each part of a section path, depending on the content and the format of the INI file.

## THE FORMATTING FUNCTIONS

The functions `ini_unquote()`, `ini_collapse_array()` and `ini_split_array()` change the content of the given string. It is important to point out that the edit is always performed within the length of the buffer given.

The behavior of these functions depends on the format given. In particular, using `ini_unquote()` as model one obtains the following scheme:

1.	**Condition:** `!format.no_single_quotes && !format.no_double_quotes && format.multiline_entries != INI_NO_MULTILINE`<br />
	⇒ **Escape sequences:** `\\`, `\"`, `\'`<br />
	⇒ **Behavior of `ini_unquote()`:** `\\`, `\'` and `\"` will be unescaped, all unescaped single and double quotes will be removed,&nbsp;then the new length of the string will be returned.
2.	**Condition:** `format.no_double_quotes`<br />
	⇒ **Escape sequences:** `\\`, `\'`<br />
	⇒ **Behavior of `ini_unquote()`:** `\\` and `\'` will be unescaped, all unescaped single quotes will be removed,&nbsp;then the new length of the string will be returned.
3.	**Condition:** `format.no_single_quotes`<br />
	⇒ **Escape sequences:** `\\`, `\"`<br />
	⇒ **Behavior of `ini_unquote()`:** `\\` and `\"` will be unescaped, all unescaped double quotes will be removed,&nbsp;then the new length of the string will be returned.
4.	`format.no_single_quotes && format.no_double_quotes && format.multiline_entries == INI_NO_MULTILINE`<br />
	⇒ **Escape sequences:** No escape sequences<br />
	⇒ **Behavior of `ini_unquote()`:** No changes will be performed, only the length of the string will be counted and returned.

## STRING COMPARISONS

In order to perform comparisons between string the function `ini_string_match_ss()`, `ini_string_match_si()` and `ini_string_match_ii()` are available. The function `ini_string_match_ss()` compares two simple strings, the function `ini_string_match_si()` compares a simple string with an unparsed INI string, and the function `ini_string_match_ii()` compares two unparsed INI strings. INI strings are the strings typically dispatched by `load_ini_file()` and `load_ini_path()`, which may contain quotes and the three escaping sequences `\\`, `\'`, `\"`. Simple strings are user-given strings or the result of `ini_unquote()`.

As a consequence, the functions `ini_string_match_si()`, `ini_string_match_ii()` do not perform literal comparisons of equality between strings. For example, in the following (absurd) INI file the two keys `foo` and `hello` belong to the same section named `this is a double quotation mark: "!` (after parsed by `ini_unquote()`).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

[this is a double quotation mark: \"!]

foo = bar

[this is a double quotation mark: '"'!]

hello = world

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Therefore...

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

char
	string1[] = "This is a double quotation mark: \\\"!",
	string2[] = "This is a double quotation mark: \'\"\'!";

printf("%s\n", ini_string_match_ii(string1, string2, my_format) ? "They match" : "They don't match");	// "They match"

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The three functions `ini_string_match_ss()`, `ini_string_match_si()`, `ini_string_match_ii()` perform case-sensitive or case-insensitive comparisons depending on the format given.

Remember that the multiline escaping sequence (`/\\(?:\n\r?|\r\n?)/`) is **not** considered as such in INI strings, since this is the only escaping sequence automatically unescaped by **libconfini** _before_ each dispatch.

## CODE CONSIDERATIONS

### RETURN VALUES

The functions `load_ini_file()`, `load_ini_path()`, `ini_array_foreach()` and `ini_split_array()` require some listeners defined by the user. Such listeners must return an `int` value. When this is non-zero the caller function is interrupted, its loop stopped, and a non-zero value is returned by the caller as well.

The functions `load_ini_file()` and `load_ini_path()` return a non-zero value also if the INI file, for any reason, has not been completely parsed (see `enum` `#ConfiniErrorNo`). Therefore, in order to be able to distinguish between internal errors and user-generated interruptions the flag `CONFINI_ERROR` can be used.

For instance, in the following example the `f_foreach()` listener returns a non-zero value if a key named `password` with a value that equals `Hello world` is found. Hence, using the flag `CONFINI_ERROR`, the code below distinguishes a non-zero value generated by the listener from a non-zero value generated by a parsing error in `load_ini_path()`'s return value.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#include <stdio.h>
#include <confini.h>

static int passfinder (IniDispatch *disp, void *v_membid) {

  /* Search for `password = "Hello world"` in the INI file */
  if (
    ini_string_match_si("password", disp->data, disp->format)
    && ini_string_match_si("Hello world", disp->value, disp->format)
  ) {

    *((signed int *) v_membid) = disp->dispatch_id;
    return 1;

  }

  return 0;

}

int main () {

  /* Load INI file */
  int membid, retval = load_ini_path(
    "my_file.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    passfinder,
    &membid
  );

  /* Check for errors */
  if (retval & CONFINI_ERROR) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  /* Check if `load_ini_path()` has been interrupted by `passfinder()` */
  retval  ==  CONFINI_EFEINTR ?
                printf("We found it! It's the INI element number #%d!\n", membid)
              :
                printf("We didn't find it :-(\n");

  return 0;

}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### OTHER GLOBAL SETTINGS

Besides the two global variables `#INI_IMPLICIT_VALUE` and `#INI_IMPLICIT_V_LEN`, a third variable named `#INI_INSENSITIVE_LOWERCASE` tells **libconfini** whether to dispatch key names and section paths lower-case or not in case-insensitive INI files.

As with the other global variables, the variable `#INI_INSENSITIVE_LOWERCASE` can be declared at the beginning of your code:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

#define FALSE 0
#define TRUE 1

#include <confini.h>

int INI_INSENSITIVE_LOWERCASE = FALSE;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alternatively, it can be set through the function `ini_dispatch_case_insensitive_lowercase()` without being explicitly declared.

When the variable `#INI_INSENSITIVE_LOWERCASE` is set to any non-zero value, **libconfini** will always convert all ASCII letters to lowercase (except within values) -- _even when these are enclosed within quotes_ -- but will **not** convert UTF-8 code points to lowercase (for instance, `Ā` will not be rendered as `ā`, but will be rather rendered verbatim). _In general it is a good practice to use UTF-8 within values, but to use ASCII only within keys names and sections names._

### THREAD SAFETY

Depending on the format of the INI file, **libconfini** may use up to three global variables (`#INI_IMPLICIT_VALUE`, `#INI_IMPLICIT_V_LEN` and `#INI_INSENSITIVE_LOWERCASE`). In order to be thread-safe these three variables (if needed) should be defined only once (either directly, or through their modifier functions `ini_set_implicit_value()` and `ini_dispatch_case_insensitive_lowercase()`), or otherwise a mutex logic must be introduced.

Apart from the three variables above, each parsing allocates and frees its own memory, therefore the library must be considered thread-safe.

### ERROR EXCEPTIONS

The philosophy of **libconfini** is to parse as much as possible without generating error exceptions. No parsing errors are returned once an INI file has been correctly allocated into the stack, with the exception of the _out-of-range_ error `CONFINI_EFEOOR` (see `enum` `#ConfiniErrorNo`), whose meaning is that the loop is for unknown reasons longer than expected -- this error is possibly generated by the presence of bugs in the library's code and should never be returned (please [contact me](https://github.com/madmurphy/libconfini/issues) if this happens).

When an INI node is wrongly written in respect to the format given, it is dispatched verbatim as an `INI_UNKNOWN` node -- see `enum` `#IniNodeType`.

In order to avoid error exceptions, strings containing an unterminated quote will always be treated as if they had a virtual quote as their last + 1 character. For example,

~~~~~~~~~~{.ini}

foo = "bar

~~~~~~~~~~

will always determine the same behavior as if it were

~~~~~~~~~~~{.ini}

foo = "bar"

~~~~~~~~~~~

### PERFORMANCE

The algorithms used by **libconfini** stand in a delicate equilibrium between flexibility, speed and code readability, with flexibility as primary target. Performance can vary with the format used to parse an INI file, but in most of the cases is not a concern.

One can measure the performance of the library by doing something like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

/* Please create an INI file large enough and call it "big_ini_file.conf" */

#include <stdio.h>
#include <confini.h>
#include <time.h>

static int get_ini_size (IniStatistics *statistics, void *user_data) {
  *((unsigned long int *) user_data) = statistics->bytes;
  return 0;
}

static int empty_listener (IniDispatch *dispatch, void *user_data) {
  return 0;
}

int main () {
  unsigned long int bytes;
  double seconds;
  clock_t start, end;
  IniFormat my_format = INI_DEFAULT_FORMAT;
  start = clock();
  if (load_ini_path("big_ini_file.conf", my_format, get_ini_size, empty_listener, &bytes)) {
    return 1;
  }
  end = clock();
  seconds = (end - start) / (double) CLOCKS_PER_SEC;
  printf(
    "%d bytes parsed in %f seconds.\n"
    "Number of bytes parsed per second: %f\n",
    bytes, seconds, bytes / seconds
  );
  return 0;
}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By changing the properties of the variable `my_format` on the code above you may obtain different results.

On my old laptop **libconfini** seems to parse around 20 MB per second using the model format `INI_DEFAULT_FORMAT`. Whether this is enough for you or not, that depends on your needs.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

54692353 bytes parsed in 2.692092 seconds.
Number of bytes parsed per second: 20315930.139089

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

