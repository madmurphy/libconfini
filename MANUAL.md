Library Functions Manual {#libconfini}
======================================


## DESCRIPTION

**libconfini** is a simple INI parsing library with the ability to read disabled entries (i.e., valid entries nested in comments). **libconfini** does not store the data read from an INI file, but rather dispatches it, formatted, to a custom listener.

The code is written in C (C99) and does not depend on any particular library, except for the C standard libraries **stdio.h**, **stdlib.h** and **stdint.h**.

If you want to start to learn directly from the code, you can find partially self-documented sample usages of **libconfini** under `/usr/share/doc/libconfini/examples/`.


## WHAT IS AN INI FILE

INI files were introduced with the early versions of Microsoft Windows, where the .ini file name extension stood for INItialization. An INI file may be considered as a string representation of a tree object, with new lines used as delimiters between nodes. A typical INI file is a plain text file looking like the following example:

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

During the years, several interpretations of INI files appeared. In some implementations the colon character (`:`) has been adopted as delimiter between keys and values instead of the classic equals sign (a typical example under GNU/Linux is `/etc/nsswitch.conf`), in other implementations the space (`/[ \t\v\f]+/` or `/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`) has been used instead (see for example `/etc/host.conf`), and so on.

Equals sign used as delimiter between keys and values:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# example1.ini

home = Champ de Mars, 5 Avenue Anatole
city = Paris
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Colon sign used as delimiter between keys and values:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# example2.ini

home: Champ de Mars, 5 Avenue Anatole
city: Paris
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Space sequence used as delimiter between keys and values:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# example3.ini

home	Champ de Mars, 5 Avenue Anatole
city	Paris
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This library has been born as a general INI parser for GNU, so the support of most part of INI dialects has been implemented within it.

Especially in Microsoft Windows a more radical syntax variation has been implemented: the use of the semicolon, instead of new lines, as delimiter between nodes, as in the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# delivery.conf

; general options

location=Colosseum;place=Rome;[sender] name=John Smith;
email=john.smith@example.com;[receiver] name=Mario Rossi; # He's a fat guy
email=mario.rossi@example.com
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For several reasons the use of semicolon as node delimiter is not (and will never be) supported by **libconfini**.


### KEYS

A **key element** is identified as a string placed after a new line and followed by a delimiter -- typically the equals sign (`=`) or the colon sign (`:`) or a space sequence (`/\s+/`) -- which is followed by a value, which is followed by a new line or an inline comment. 

Both the **key part** and the **value part** may be enclosed within quotes (`'` or `"`):

~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
foo = 'bar'
"hello" = world
"artist" = "Pablo Picasso"
~~~~~~~~~~~~~~~~~~~~~~~~~~

The **key part** can contain any character, except the delimiter (which may be enclosed within quotes for not beeing considered as such). In multi-line formats internal new line sequences must be escaped (`/\\(?:\n\r?|\r\n?)/`).

If the **key part** part is missing **libconfini** considers the element of _unknown type_ (example: `= foo`). If the **value part** is missing the key element is considered empty (example: `foo =`). If the delimiter is missing (and therefore the **value part** as well), according to some formats the key element is considered to be an _implicit key_ -- typically representing the boolean `TRUE` (example: `foo`). For instance, in the following example from `/etc/pacman.conf`, `IgnorePkg` is an empty key, while `Color` is an implicit key (representing a `TRUE` boolean -- i.e., `Color = YES`):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
HoldPkg = pacman glibc
Architecture = auto
IgnorePkg =
Color
SigLevel = Required DatabaseOptional
LocalFileSigLevel = Optional
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The **value** part can contain typed data, usually: a boolean (booleans supported by **libconfini** are: `NO`/`YES`, `FALSE`/`TRUE`, `0`/`1`), a string, a number, or an array (typically with commas or spaces as delimiters between members -- examples: `paths = /etc, /usr, "/home/john/Personal Data"` or `paths = /etc /usr "/home/john/Personal Data"`). In multi-line formats internal new line sequences must be escaped (`/\\(?:\n\r?|\r\n?)/`).

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

A **section** might be imagined like a directory. A **section path** is identified as the string `"$1"` in the regular expression `/(?:^|\n)[ \t\v\f]*\[[ \t\v\f]*([^\]]*)[ \t\v\f]*\]/` globally applied to an INI file. A section path expresses nesting through the “dot” character, as in the following example:

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

Comments may in theory be multi-line, following the same syntax of multi-line disabled entries (see below). This is usually of little utility, except for inline comments that you want to make sure will refer to the previous entry:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
play1 = The Tempest

play2 = Twelfth Night # If music be the food of love, play on;      \
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### DISABLED ENTRIES

A disabled entry is either a section or a key that has been nested inside a comment as its only child. Inline comments cannot represent disabled entries. According to some formats disabled entries can be multi-line, using `/\\(?:\n\r?|\r\n?)[\t \v\f]*[;#]/` as multi-line escaping sequence. For example:

~~~~~~~~~~~~~~~~{.ini}
#this = is\
 #a\
    #multi-line\
#disabled\
  #entry
~~~~~~~~~~~~~~~~


### ESCAPING SEQUENCES

In order to maximize the flexibility of the data, only four escaping sequences are supported by **libconfini**: `\'`, `\"`, `\\` and the multi-line escaping sequence (`/\\(?:\n\r?|\r\n?)/`).

The first three escaping sequences are left untouched by all functions except `ini_string_parse()` and `ini_unquote()`. Nevertheless, the characters `'`, `"` and `\` can determine different behaviors during the parsing depending on whether they are escaped or unescaped. For instance, the string `johnsmith !"` in the following example will not be parsed as a comment:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
[users.jsmith]

comment = "hey! have a look at my hashtag #johnsmith !"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A particular case of escaping sequence is the multi-line escaping sequence (`/\\(?:\n\r?|\r\n?)/`), which in multi-line INI files gets _immediately unescaped by **libconfini**_.

~~~~~~~~~~~{.ini}
foo = this\
is\
a\
multi-line\
value
~~~~~~~~~~~


## READ AN INI FILE

The syntaxes of **libconfini**'s parsing functions are:

\#1 Using a pointer to a `FILE` structure:

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

\#2 Using a path:

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
* `f_foreach` is the callback function that will be repeatedly invoked for each member of the INI file - it can be `NULL`
* `user_data` is a pointer to a custom argument -- it can be `NULL`

The function `f_init()` will be invoked with two arguments:

* `statistics` -- a pointer to an `IniStatistics` object containing some properties about the file read
  (like its size in bytes and the number of its members)
* `user_data` -- a pointer to the custom argument previously passed to the `load_ini_file()` / `load_ini_path()` functions

The function `f_foreach()` will be invoked with two arguments:

* `dispatch` -- a pointer to an `IniDispatch` object containing the parsed member of the INI file
* `user_data` -- a pointer to the custom argument previously passed to the `load_ini_file()` / `load_ini_path()` functions

Both functions `load_ini_file()` and `load_ini_path()` will return zero if the INI file has been completely parsed, non-zero otherwise.


## BASIC EXAMPLES

\#1:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/* examples/topics/load_ini_file.c */

#include <stdio.h>
#include <confini.h>

int callback (IniDispatch *dispatch, void *v_null) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %d\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  FILE * const ini_file = fopen("file.ini", "r");

  if (ini_file == NULL) {

    fprintf(stderr, "File doesn't exist :-(\n");
    return 1;

  }

  if (load_ini_file(ini_file, INI_DEFAULT_FORMAT, NULL, callback, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  fclose(ini_file);

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

\#2:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/* examples/topics/load_ini_path.c */

#include <stdio.h>
#include <confini.h>

int callback (IniDispatch *dispatch, void *v_null) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %d\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  if (load_ini_path("file.ini", INI_DEFAULT_FORMAT, NULL, callback, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## HOW IT WORKS

The function `load_ini_path()` is a shortcut to the function `load_ini_file()` that requires a path instead of a `FILE` struct.

The function `load_ini_file()` dynamically allocates at once the whole INI file into the heap, and the two structures `IniStatistics` and `IniDispatch` into the stack. All the members of the INI file are then dispatched to the custom listener `f_foreach()`. Finally the allocated memory gets automatically freed.

Because of this mechanism _it is very important that all the dispatched data be **immediately** copied by the user (when needed), and no pointers to the passed data be saved_: after the end of the functions `load_ini_file()` / `load_ini_path()` all the allocated data will be destroyed indeed.

Within a dispatching cycle, the structure containing each dispatch (`IniDispatch *dispatch`) is always the same `struct` that gets constantly updated with new information.


## THE `IniFormat` BITFIELD

For a correct use of this library it is fundamental to understand the `IniFormat` bitfield. **libconfini** has been born as a general INI parser, with the main purpose of _being able to understand INI files written by other programs_ (see @ref readme), therefore some flexibility was required.

When an INI file is parsed it is parsed according to a format. The `IniFormat` bitfield is a description of such format.


### THE MODEL FORMAT

A model format named `#INI_DEFAULT_FORMAT` is available.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_format;

my_format = INI_DEFAULT_FORMAT;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The code above corresponds to:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define NO 0
#define YES 1

IniFormat my_format;

my_format.delimiter_symbol = INI_EQUALS;	/* or `my_format.delimiter_symbol = '='` */
my_format.semicolon_marker = INI_DISABLED_OR_COMMENT;
my_format.hash_marker = INI_DISABLED_OR_COMMENT;
my_format.multiline_nodes = INI_MULTILINE_EVERYWHERE;
my_format.case_sensitive = NO;
my_format.no_spaces_in_names = NO;
my_format.no_single_quotes = NO;
my_format.no_double_quotes = NO;
my_format.implicit_is_not_empty = NO;
my_format.do_not_collapse_values = NO;
my_format.preserve_empty_quotes = NO;
my_format.no_disabled_after_space = NO;
my_format.disabled_can_be_implicit = NO,
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### THE `#IniFormatNum` DATA TYPE

Each format can be represented also as a univocal 24-bit unsigned integer. In order to convert an `IniFormat` to an unsigned integer and vice versa the functions `ini_fton()` and `ini_ntof()` are available.

For instance, imagine we want to create a format as close as possible to the typical Windows INI files. Probably we would define our format as follows:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define NO 0
#define YES 1

IniFormat my_format = {
	.delimiter_symbol = INI_EQUALS,
	.semicolon_marker = INI_IGNORE,
	.hash_marker = INI_IS_NOT_A_MARKER,
	.multiline_nodes = INI_NO_MULTILINE,
	.case_sensitive = NO,
	.no_spaces_in_names = YES,
	.no_single_quotes = NO,
	.no_double_quotes = NO,
	.implicit_is_not_empty = NO,
	.do_not_collapse_values = NO,
	.preserve_empty_quotes = NO,
	.no_disabled_after_space = NO,
	.disabled_can_be_implicit = NO
};

IniFormatNum my_format_num = ini_fton(my_format);

printf("Format No. %d\n", my_format_num); // "Format No. 48701"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The function `ini_fton()` tells us that this format is univocally the format No. 48701. The function `ini_ntof()` gives us then a shortcut to construct the very same format using its format number. Hence, the code above corresponds to:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_format = ini_ntof(48701);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## THE `IniStatistics` AND `IniDispatch` STRUCTURES

When the functions `load_ini_file()` and `load_ini_path()` read an INI file, they dispatch the file content to the `f_foreach()` listener. Before the dispatching begins some statistics about the parsed file can be dispatched to the `f_init()` listener (if this is non-`NULL`).

The information passed to `f_init()` is passed through an `IniStatistics` structure, while the information passed to `f_foreach()` is passed through an `IniDispatch` structure.


## RENDERING

The output strings dispatched by **libconfini** will follow some formatting rules depending on their role within the INI file. First, the multi-line sequences will be unescaped, then

* **Section paths** will be rendered according to ECMAScript `section_name.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/\s+/g, " ")` -- within single or double quotes, if active, the text will be rendered verbatim
* **Key names** will be rendered according to ECMAScript `key_name.replace(/^[\n\r]\s*|\s+/g, " ")` -- within single or double quotes, if active, the text will be rendered verbatim
* **Values**, if `format.do_not_collapse_values` is active, will only be cleaned of spaces at the beginning and at the end, otherwise, will be rendered through the same algorithm used for key names (with the difference that, if `format.preserve_empty_quotes` is set to `1`, empty quotes surrounded by spaces will be preserved).
* **Comments**: if multi-line, ECMAScript `comment_string.replace(/(^|\n\r?|\r\n?)[ \t\v\f]*[#;]+/g, "$1")`; otherwise, ECMAScript `comment_string.replace(/^[ \t\v\f]*[#;]+/, "")`.
* **Unknown nodes** will be rendered verbatim.

The strings passed with each dispatch, as already said, must not be freed. _Nevertheless, before being copied or analyzed they can be edited, **with some precautions**_:

1. Be sure that your edit remains within the buffer lengths given (see: `IniDispatch::d_len` and `IniDispatch::v_len`).
2. If you want to edit the content of `IniDispatch::data` and this contains a section path, the `IniDispatch::append_to` properties of its children _may_ refer to the same buffer. In this case, if you edit it, you can no more rely on its children's `IniDispatch::append_to` properties (you will not make any damage, the loop will continue just fine: so if you think you are going to never use the property `IniDispatch::append_to` just do it); alternatively, use `strdup()`. If, instead, `IniDispatch::data` contains a key name or a comment, no other dispatch will share this buffer, so feel free to edit it before it gets lost.
3. Regarding `IniDispatch::value`, the buffer is never shared between dispatches, so feel free to edit it.
4. Regarding `IniDispatch::append_to`, this buffer is likely to be shared with other dispatches. Again, you will not destroy the world nor generate errors, but you will make the next `IniDispatch::append_to`s useless. Therefore **the property `IniDispatch::append_to` should be considered read-only** -- this is just a logical imposition (and this is why `IniDispatch::append_to` is not passed as `const`). To format this field please use `strdup()`.

Typical peaceful edits are the calls of the functions `ini_array_collapse()` and `ini_string_parse()` directly on the buffer `IniDispatch::value` -- but make sure that you are not going to edit the global string `#INI_GLOBAL_IMPLICIT_VALUE`, if used (see below):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/* examples/topics/ini_string_parse.c */

#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *v_null) {

  if (dispatch->type == INI_KEY || dispatch->type == INI_DISABLED_KEY) {

    ini_string_parse(dispatch->data, dispatch->format);
    ini_string_parse(dispatch->value, dispatch->format);

  }

  printf("DATA: %s\nVALUE: %s\n", dispatch->data, dispatch->value);

  return 0;

}

int main () {

  if (load_ini_path(
    "example.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to set the value to be assigned to implicit keys, please use the `ini_global_set_implicit_value()` function. A _zero-length `TRUE`-boolean_ is usually a good choice:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
ini_global_set_implicit_value("YES", 0);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alternatively, instead of `ini_global_set_implicit_value()` you can manually define at the beginning of your code the two global variables `#INI_GLOBAL_IMPLICIT_VALUE` and `#INI_GLOBAL_IMPLICIT_V_LEN`, which will be retrieved by **libconfini**:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <confini.h>

char *INI_GLOBAL_IMPLICIT_VALUE = "YES";
size_t INI_GLOBAL_IMPLICIT_V_LEN = 3;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If not defined elsewhere, these variables are respectively `NULL` and `0` by default.

After having set the value to be assigned to implicit key elements and having enabled `IniFormat::implicit_is_not_empty` in the format, it is possible to test whether a dispatched key is implicit or not by comparing the address of its `value` property with the global variable `#INI_GLOBAL_IMPLICIT_VALUE`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/* examples/topics/ini_global_set_implicit_value.c */

#include <stdio.h>
#include <confini.h>

#define NO 0
#define YES 1

int ini_listener (IniDispatch *dispatch, void *v_null) {

  if (dispatch->value == INI_GLOBAL_IMPLICIT_VALUE) {

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

  ini_global_set_implicit_value("[implicit default value]", 0);

  /* Without setting this, implicit keys will be anyway considered empty: */
  my_format.implicit_is_not_empty = YES;

  if (load_ini_path("example.conf", my_format, NULL, ini_listener, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## FORMATTING THE VALUES

Once your listener starts to receive the parsed data you may want to parse and better format the `value` part of key elements. The following functions may be useful for this purpose:

* `ini_string_parse()`
* `ini_array_get_length()`
* `ini_array_collapse()`
* `ini_array_foreach()`
* `ini_array_split()`
* `ini_get_bool()`
* `ini_get_lazy_bool()`

Together with the functions listed above the following links are available, in case you don't want to include `<stdlib.h>` in your source:

* `ini_get_int()` = [`atoi()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi)
* `ini_get_lint()` = [`atol()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol)
* `ini_get_llint()` = [`atoll()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll)
* `ini_get_float()` = [`atof()`](http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof)


## FORMATTING THE KEY NAMES

The function `ini_unquote()` may be useful for key names enclosed within quotes. This function is very similar to `ini_string_parse()`, except that does not collapse the spaces surrounding empty quotes after the latter have been removed -- this is never necessary since empty quotes surrounded by spaces in key and section names are always collapsed before the dispatching.


## FORMATTING THE SECTION PATHS

In order to retrieve the parts of a section path, the functions `ini_array_get_length()`, `ini_array_foreach()` and `ini_array_split()` can be used with `'.'` as delimiter. Note that section paths dispatched by **libconfini** are _always_ collapsed arrays, therefore calling the function `ini_array_collapse()` on them will have no effects.

It may be required that the function `ini_unquote()` be applied to each part of a section path, depending on the content and the format of the INI file.


## CODE CONSIDERATIONS


### RETURN VALUES

The functions `load_ini_file()`, `load_ini_path()`, `ini_array_foreach()` and `ini_array_split()` require some listeners defined by the user. Such listeners must return an `int` value. When this is non-zero the caller function is interrupted, its loop stopped, and a non-zero value is returned by the caller as well.

The functions `load_ini_file()` and `load_ini_path()` return a non-zero value also if the INI file, for any reason, has not been completely parsed (see `enum` `#ConfiniInterruptNo`). Therefore, in order to be able to distinguish between internal errors and user-generated interruptions the flag `#CONFINI_ERROR` can be used.

For instance, in the following example the `f_foreach()` listener returns a non-zero value if a key named `password` with a value that equals `Hello world` is found. Hence, using the flag `#CONFINI_ERROR`, the code below distinguishes a non-zero value generated by the listener from a non-zero value generated by a parsing error in `load_ini_path()`'s return value.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/* examples/topics/ini_string_match_si.c */

#include <stdio.h>
#include <confini.h>

static int passfinder (IniDispatch *disp, void *v_membid) {

  /* Search for `password = "Hello world"` in the INI file */
  if (
    ini_string_match_si("password", disp->data, disp->format)
    && ini_string_match_si("Hello world", disp->value, disp->format)
  ) {

    *((size_t *) v_membid) = disp->dispatch_id;
    return 1;

  }

  return 0;

}

int main () {

  size_t membid;

  /* Load INI file */
  int retval = load_ini_path(
    "example.conf",
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
  retval  ==  CONFINI_FEINTR ?
                printf("We found it! It's the node No. %d!\n", membid)
              :
                printf("We didn't find it :-(\n");

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### THE FORMATTING FUNCTIONS

The functions `ini_string_parse()`, `ini_unquote()`, `ini_array_collapse()` and `ini_array_split()` change the content of the given string. It is important to point out that the edit is always performed within the length of the buffer given.

The behavior of these functions depends on the format given. In particular, using `ini_string_parse()` as model one obtains the following scheme:

1. Condition: `!format.no_single_quotes && !format.no_double_quotes && format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`, `\"`, `\'`<br />
   ⇒ Behavior of `ini_string_parse()`: `\\`, `\'` and `\"` will be unescaped, all unescaped single and double quotes will be removed, then the new length of the string will be returned.
2. Condition: `!format.no_single_quotes && format.no_double_quotes && format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`, `\'`<br />
   ⇒ Behavior of `ini_string_parse()`: `\\` and `\'` will be unescaped, all unescaped single quotes will be removed, then the new length of the string will be returned.
3. Condition: `format.no_single_quotes && !format.no_double_quotes && format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`, `\"`<br />
   ⇒ Behavior of `ini_string_parse()`: `\\` and `\"` will be unescaped, all unescaped double quotes will be removed, then the new length of the string will be returned.
4. Condition: `format.no_single_quotes && format.no_double_quotes && format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`<br />
   ⇒ Behavior of `ini_string_parse()`: only `\\` will be unescaped, then the new length of the string will be returned.
5. Condition: `format.no_single_quotes && format.no_double_quotes && format.multiline_nodes == INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: No escape sequences<br />
   ⇒ Behavior of `ini_string_parse()`: Spaces at the beginning and at the end of the string will be removed, then the new length of the string will be returned.


### STRING COMPARISONS

In order to perform comparisons between strings the function `ini_string_match_ss()`, `ini_string_match_si()` and `ini_string_match_ii()` are available. The function `ini_string_match_ss()` compares two simple strings, the function `ini_string_match_si()` compares a simple string with an unparsed INI string, and the function `ini_string_match_ii()` compares two unparsed INI strings. INI strings are the strings typically dispatched by `load_ini_file()` and `load_ini_path()`, which may contain quotes and the three escaping sequences `\\`, `\'`, `\"`. Simple strings are user-given strings or the result of `ini_string_parse()`.

As a consequence, the functions `ini_string_match_si()`, `ini_string_match_ii()` do not perform literal comparisons of equality between strings. For example, in the following (absurd) INI file the two keys `foo` and `hello` belong to the same section named `this is a double quotation mark: "!` (after being parsed by `ini_string_parse()`).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
[this is a double quotation mark: \"!]

foo = bar

[this is a double quotation mark: '"'!]

hello = world
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Therefore...

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
char
	string1[] = "This is a double quotation mark: \\\"!",
	string2[] = "This is a double quotation mark: '\"'!";

printf(
	"%s\n",

	ini_string_match_ii(string1, string2, my_format) ?
		"They match"
	:
		"They don't match"
);	// "They match"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The three functions `ini_string_match_ss()`, `ini_string_match_si()`, `ini_string_match_ii()` perform case-sensitive or case-insensitive comparisons depending on the format given. UTF-8 codepoints out of the ASCII range are always compared case-sensitive.

Note that, within INI strings, empty quotes and spaces out of quotes are always collapsed during comparisons. Furthermore, remember that the multi-line escaping sequence (`/\\(?:\n\r?|\r\n?)/`) is _not_ considered as such in INI strings, since this is the only escaping sequence automatically unescaped by **libconfini** _before_ each dispatch.


### SIZE OF THE DISPATCHED DATA

Within an INI file it is granted that if one sums together all the `(dispatch->d_len + 1)` and all the `(dispatch->v_len > 0 ? dispatch->v_len + 1 : 0)` received, the result will always be less-than or equal-to `(statistics->bytes + 1)` (where `+ 1` represents the NUL terminators). **If one adds to this also all the `dispatch->at_len` properties, or if the `dispatch->v_len` properties of implicit keys are non-zero, the sum may exceed it.** This can be relevant or irrelevant depending on your code.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <stdio.h>
#include <confini.h>

struct size_check {
  size_t bytes, buff_lengths;
};

int ini_init (IniStatistics *stats, void *v_check_struct) {

  ((struct size_check *) v_check_struct)->bytes = stats->bytes;
  ((struct size_check *) v_check_struct)->buff_lengths = 0;
  return 0;

}

int ini_listener (IniDispatch *this, void *v_check_struct) {

  ((struct size_check *) v_check_struct)->buff_lengths += this->d_len + 1 +
    (this->v_len ? this->v_len + 1 : 0);

  return 0;

}

int main () {

  struct size_check check;

  if (load_ini_path(
    "my_file.ini",
    INI_DEFAULT_FORMAT,
    ini_init,
    ini_listener,
    &check
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  printf(

    "The file is %d bytes large.\n\nThe sum of the lengths of all "
    "IniDispatch::data buffers plus the lengths of all non-empty "
    "IniDispatch::value buffers is %d.\n",

    check.bytes, check.buff_lengths

  );

  /* `INI_GLOBAL_IMPLICIT_V_LEN` is 0 and not even used, so this cannot
     happen: */

  if (check.buff_lengths > check.bytes) {

    fprintf(stderr, "The end is near!");
    return 1;

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### OTHER GLOBAL SETTINGS

Besides the two global variables `#INI_GLOBAL_IMPLICIT_VALUE` and `#INI_GLOBAL_IMPLICIT_V_LEN`, a third variable named `#INI_GLOBAL_LOWERCASE_MODE` tells **libconfini** whether to dispatch key names and section paths lower-case or not in case-insensitive INI files.

As with the other global variables, you can declare the variable `#INI_GLOBAL_LOWERCASE_MODE` at the beginning of your code:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define FALSE 0
#define TRUE 1

#include <confini.h>

_Bool INI_GLOBAL_LOWERCASE_MODE = FALSE;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alternatively, this variable can be set through the function `ini_global_set_lowercase_mode()` without being explicitly declared.

When the variable `#INI_GLOBAL_LOWERCASE_MODE` is set to `TRUE`, **libconfini** will always convert to lowercase _all_ ASCII letters of key and section names in case-insensitive formats -- _even when these are enclosed within quotes_ -- but will **not** convert UTF-8 code points to lowercase (for instance, `Ā` will not be rendered as `ā`, but will be rather rendered verbatim). _In general it is a good practice to use UTF-8 within values, but to use ASCII only within keys names and sections names._

Normally `#INI_GLOBAL_LOWERCASE_MODE` does not need to be set to `TRUE`, since string comparisons made by libconfini are always either case-sensitive or case-insensitive depending on the format given.


### THREAD SAFETY

Depending on the format of the INI file, **libconfini** may use up to three global variables (`#INI_GLOBAL_IMPLICIT_VALUE`, `#INI_GLOBAL_IMPLICIT_V_LEN` and `#INI_GLOBAL_LOWERCASE_MODE`). In order to be thread-safe these three variables (if needed) must be defined only once (either directly, or through their modifier functions `ini_global_set_implicit_value()` and `ini_global_set_lowercase_mode()`), or otherwise a mutex logic must be introduced.

Apart from the three variables above, each parsing allocates and frees its own memory and every function is fully reentrant, therefore the library must be considered thread-safe.


### ERROR EXCEPTIONS

The philosophy of **libconfini** is to parse as much as possible without generating error exceptions. No parsing errors are returned once an INI file has been correctly allocated into the stack, with the exception of the _out-of-range_ error `#CONFINI_EFEOOR` (see `enum` `#ConfiniInterruptNo`), whose meaning is that the loop is for unknown reasons longer than expected -- this error is possibly generated by the presence of bugs in the library's code and should never be returned (please [contact me](https://github.com/madmurphy/libconfini/issues) if this happens).

When an INI node is wrongly written in respect to the format given, it is dispatched verbatim as an `#INI_UNKNOWN` node -- see `enum` `#IniNodeType`.

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

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/* Please create an INI file large enough and call it "big_ini_file.conf" */

#include <stdio.h>
#include <confini.h>
#include <time.h>

static int get_ini_size (IniStatistics *statistics, void *v_bytes) {
  *((size_t *) v_bytes) = statistics->bytes;
  return 0;
}

static int empty_listener (IniDispatch *dispatch, void *v_bytes) {
  return 0;
}

int main () {
  size_t bytes;
  double seconds;
  clock_t start, end;
  IniFormat my_format = INI_DEFAULT_FORMAT;
  start = clock();
  if (load_ini_path(
    "big_ini_file.conf",
    my_format,
    get_ini_size,
    empty_listener,
    &bytes
  )) {
    return 1;
  }
  end = clock();
  seconds = (double) (end - start) / CLOCKS_PER_SEC;
  printf(
    "%d bytes parsed in %f seconds.\n"
    "Number of bytes parsed per second: %f\n",
    bytes, seconds, bytes / seconds
  );
  return 0;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By changing the properties of the variable `my_format` on the code above you may obtain different results.

On my old laptop **libconfini** seems to parse around 20 MiB per second using the model format `#INI_DEFAULT_FORMAT`. Whether this is enough for you or not, that depends on your needs.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
54692353 bytes parsed in 2.575119 seconds.
Number of bytes parsed per second: 21238767.218136
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## INI SYNTAX CONSIDERATIONS


### COMMENT OR DISABLED ENTRY?

I can hardly imagine a reason to be interested in disabled entries if not for writing a GUI editor for INI files. However, if this is the case and you are not using **libconfini** like normal people do, you might wonder how to ensure that disabled entries and comments be always parsed without ambiguity.

In most of the cases **libconfini** is smart enough to distinguish a disabled entry from a comment. However some INI files can be tricky and might require some workarounds. For instance, imagine to have the following INI file:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# INI key/value delimiter: = (everywhere)

[some_section]

hello = world

;foo = bar

##now=Sunday April 3rd, 2016
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

And imagine that for unknown reasons the author of the INI file wanted only `;foo = bar` to be considered as a disabled entry, and the first and last lines as normal comments.

If we tried to parse it using the model format `#INI_DEFAULT_FORMAT` using the following code

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *v_null) {
  printf(
    "NODE #%d - TYPE: %d, DATA: '%s', VALUE: '%s'\n",
    dispatch->dispatch_id, dispatch->type, dispatch->data, dispatch->value
  );
  return 0;
}

int main () {
  IniFormat my_format = INI_DEFAULT_FORMAT;
  printf(":: Content of 'ambiguous.conf' ::\n\n");
  if (load_ini_path(
    "ambiguous.conf",
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

we would obtain the following result:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:: Content of 'ambiguous.conf' ::

NODE #0 - TYPE: 6, DATA: 'INI key/value delimiter:', VALUE: '(everywhere)'
NODE #1 - TYPE: 3, DATA: 'some_section', VALUE: ''
NODE #2 - TYPE: 2, DATA: 'hello', VALUE: 'world'
NODE #3 - TYPE: 6, DATA: 'foo', VALUE: 'bar'
NODE #4 - TYPE: 4, DATA: 'now=Sunday April 3rd, 2016', VALUE: ''
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As one can see, all comments but `now=Sunday April 3rd, 2016` would be parsed as disabled entries -- which is not what the author intended. Therefore, if you want to ensure that such INI file is parsed properly, you can follow two possible approaches.

**1. Intervene on the INI file.** The reason why `now=Sunday April 3rd, 2016` has been properly parsed as a comment -- despite it really looks like a disabled entry -- is because it has been nested in a comment block opened by more than one leading comment marker (in this case the two `##`). As a general rule, _**libconfini** never parses a comment beginning with more than one leading comment marker as a disabled entry_, therefore this is the surest way to ensure that proper comments are always considered as such.

Hence, by adding one more number sign to the first comment

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
## INI key/value delimiter: = (everywhere)

[some_section]

hello = world

;foo = bar

##now=Sunday April 3rd, 2016
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

one obtains the wanted result:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:: Content of 'ambiguous.conf' ::

NODE #0 - TYPE: 4, DATA: ' INI key/value delimiter: = (everywhere)', VALUE: ''
NODE #1 - TYPE: 3, DATA: 'some_section', VALUE: ''
NODE #2 - TYPE: 2, DATA: 'hello', VALUE: 'world'
NODE #3 - TYPE: 6, DATA: 'foo', VALUE: 'bar'
NODE #4 - TYPE: 4, DATA: 'now=Sunday April 3rd, 2016', VALUE: ''
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**2. Intervene on the format.** There are cases where the INI file is automatically generated by machines (comments included), or distributed as such, and human intervention would be required on each machine-generated realease of the INI file. In these cases -- and if you are sure about the expected content of the INI file -- you can restrict the format chosen in order to parse comments and disabled entries properly. In particular, the following fields of the `IniFormat` bitfield may have an impact on the disambiguation between comments and disabled entries.

Reliable general patterns:

* `IniFormat::semicolon_marker` and `IniFormat::hash_marker` -- The imaginary author of our INI file, if one observes the latter closer, chose the semicolon symbol as the leading character for disabled entries and the hash symbol as the leading character of comments. You may exploit this difference and set your `my_format.semicolon_marker` to `#INI_DISABLED_OR_COMMENT` and your `my_format.hash_marker` to `#INI_ONLY_COMMENT` to obtain the correct result. If you believe that this solution is too artificial, think that `/etc/samba/smb.conf` and `/etc/pulse/daemon.conf` are systematically distributed using this pattern.
* `IniFormat::no_disabled_after_space` -- Setting this property to `TRUE`, due to the initial space that follows the comment marker (`# INI...`), forces `# INI key/value delimiter: = (everywhere)` to be considered as a comment. Some authors use this syntax to distinguish between comments and disabled entries (examples are `/etc/pacman.conf` and `/etc/bluetooth/main.conf`)

Temporary workarounds:

* `IniFormat::no_spaces_in_names` -- If your INI file has only comments containing more than one word and you are sure that key and section names cannot contain internal white spaces, you can set this property to `TRUE` to enhance the disambiguation.
* `IniFormat::disabled_can_be_implicit` -- This property, if set to `FALSE`, forces all comments that do not contain a key-value delimiter to never be considered as disabled entries. Despite not having an impact on our example, it has a big impact on the disambiguation algorithm used by **libconfini**. Its value in `#INI_DEFAULT_FORMAT` is set to `FALSE`.

As a general rule **libconfini** will always try to parse as a disabled entry whatever comment is allowed (by the format) to contain one. Only if this fails, the block will be dispatched as a normal comment.

