Library Functions Manual {#libconfini}
======================================


## Description

**libconfini** is a simple INI parsing library with the ability to read
disabled entries (i.e. valid entries nested in comments). **libconfini** does
not store the data read from an INI file, but rather dispatches it, formatted,
to a custom listener.

The code is written in C (C99) and does not depend on any particular library,
except for the C standard headers **stdio.h**, **stdlib.h**, **stdbool.h** and
**stdint.h**.

If you want to start learning directly from the code, you can find partially
self-documented sample usages of **libconfini** under
`/usr/share/doc/libconfini/examples`.


## What is an INI file?

INI files were introduced with the early versions of Microsoft Windows, where
the .ini file name extension stood for INItialization. An INI file can be
considered as a string representation of a tree object, with new lines used as
delimiters between nodes. A typical INI file is a plain text file looking like
the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# delivery.conf

; general options

location = Colosseum
place = Rome

[sender]
name = John Smith
email = john.smith@example.com

[receiver]
name = Mario Rossi   # He's a big guy
email = mario.rossi@example.com
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## Supported syntaxes

During the years several interpretations of INI files have appeared. In some
implementations the colon character (`:`) has been adopted as delimiter between
keys and values instead of the classic equals sign (a typical example under
GNU/Linux is `/etc/nsswitch.conf`); in other implementations, under the
influence of Unix standard configuration files, a sequence of one or more
spaces (`/[ \t\v\f]+/` or `/(?:\\(?:\n\r?|\r\n?)|[\t \v\f])+/`) has been used
instead (see for example `/etc/host.conf`).

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

**libconfini** has been born as a general INI parser for **GNU**, so the
support of most part of INI dialects has been implemented within it.

Especially in **Microsoft Windows** a more radical syntax variation has been
implemented: the use of semicolon, instead of new lines, as delimiter between
nodes, as in the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# delivery.conf

; general options

location=Colosseum;place=Rome;[sender] name=John Smith;
email=john.smith@example.com;
[receiver] name=Mario Rossi; # He's a big guy
email=mario.rossi@example.com
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For several reasons the use of semicolon as node delimiter is not (and will
never be) supported by **libconfini**.


### Keys

A **key-value element** is identified as a string placed after a new line and
followed by a key-value delimiter -- typically the equals sign (`=`) or the
colon sign (`:`) or a space sequence (`/\s+/`) -- which is followed by a value,
which is followed by a new line or an inline comment. 

Both the **key part** and the **value part** may be enclosed within quotes (`'`
or `"`):

~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
foo = 'bar'
"hello" = world
"artist" = "Pablo Picasso"
~~~~~~~~~~~~~~~~~~~~~~~~~~

The **key part** can contain any character, except the delimiter (although this
may be enclosed within quotes for not beeing considered as such). In multi-line
formats internal new line sequences must be escaped (`/\\(?:\n\r?|\r\n?)/`).

If the **key part** part is missing **libconfini** considers the element of
_unknown type_ (example: `= foo`). If the **value part** is missing the key
element is considered empty (example: `foo =`). If the delimiter is missing
(and therefore the value part as well), according to some formats the key
element is is considered to be an _implicit key_ -- typically representing the
boolean `true` (example: `foo`). For instance, in the following example from
`/etc/pacman.conf`, `IgnorePkg` is an empty key, while `Color` is an implicit
key representing a `true` boolean -- i.e. `Color = YES`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
HoldPkg = pacman glibc
Architecture = auto
IgnorePkg =
Color
SigLevel = Required DatabaseOptional
LocalFileSigLevel = Optional
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The **value** part can contain typed data, usually: a boolean (booleans
supported by **libconfini** are: `FALSE`/`TRUE`, `NO`/`YES`, `OFF`/`ON` --
case-insensitive), a string, a number, or an array (typically with commas or
spaces as delimiters between members -- examples: `paths = /etc, /usr,
/home/john/Personal Data` or `paths = /etc /usr "/home/john/Personal Data"`).
In multi-line formats internal new line sequences must be escaped
(`/\\(?:\n\r?|\r\n?)/`).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
[my_section]

my_string = "Hello world"
'my_number' = 42
my_boolean = NO
my_implicit_boolean
my_array = Asia, Africa, 'North America', South America, \
           Antarctica, Europe, Australia
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Sections

A **section** can be imagined as a directory. A **section path** is identified
as the string `"$1"` in the regular expression
`/(?:^|\n|\r)[ \t\v\f]*\[[ \t\v\f]*([^\]]*)[ \t\v\f]*\][ \t\v\f]*(?:\n|\r|$)/`
globally applied to an INI file. A section path expresses nesting using the
“dot” character, as in the following example:

~~~~~~~~~~~~~~~~~~~~{.ini}
[section]

foo = bar

[section.subsection]

foo = bar
~~~~~~~~~~~~~~~~~~~~

A section path starting with a dot expresses nesting to the previous section.
Hence the last example is equivalent to:

~~~~~~~~~~~~~{.ini}
[section]

foo = bar

[.subsection]

foo = bar
~~~~~~~~~~~~~

Keys appearing before any section path belong to a virtual _root_ node (with an
empty string as path), as the key `foo` in the following example:

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


### Comments

Comments are string segments enclosed within the sequence `/(?:^|\s)[;#]/` and
a new line sequence, as in the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# this is a comment

foo = bar       # this is an inline comment

; this is another comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Comments may in theory be multi-line, following the same syntax of multi-line
disabled entries (see below). This is usually of little utility, except for
inline comments you want to make sure will refer to the previous entry:

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

# This is also a masterpiece
play3 = The Merchant of Venice
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Disabled entries

A disabled entry is either a section or a key that has been nested inside a
comment as its only child. Inline comments cannot represent disabled entries.
According to some formats disabled entries can be multi-line, using
`/\\(?:\n\r?|\r\n?)[\t \v\f]*[;#]/` as multi-line escape sequence. For example:

~~~~~~~~~~~~~~~~~{.ini}
#this = is \
 #a \
    #multi-line \
#disabled \
  #entry
~~~~~~~~~~~~~~~~~


### Escape sequences

In order to maximize the flexibility of the data, only four escape sequences
are supported by **libconfini**: `\'`, `\"`, `\\` and the multi-line escape
sequence (`/\\(?:\n\r?|\r\n?)/`).

The first three escape sequences are left untouched by all functions except
`ini_string_parse()` and `ini_unquote()` (see below). Nevertheless, the
characters `'`, `"` and `\` can determine different behaviors during the
parsing depending on whether they are escaped or unescaped. For instance, the
string `johnsmith !"` in the following example will not be parsed as a comment:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
[users.jsmith]

comment = "hey! have a look at my hashtag #johnsmith !"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A particular case of escape sequence is the multi-line escape sequence
(`/\\(?:\n\r?|\r\n?)/`), which in multi-line INI files gets _immediately
unescaped by **libconfini**_.

~~~~~~~~~~~{.ini}
foo = this\
is\
a\
multi-line\
value
~~~~~~~~~~~


## Getting started

The API is contained in one single public header:

~~~~~~~~~~~~~~~~~~~~{.c}
#include <confini.h>
~~~~~~~~~~~~~~~~~~~~

When **libconfini** is used as a shared library, it might be wiser to include
_the versioned header_ (with only the major version number appended to the file
name), in order to ensure that the code will compile correctly even when
different major versions of the library cohabit in the same machine. This can
apply also to version 1.X.X:

~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <confini-1.h>
~~~~~~~~~~~~~~~~~~~~~~


## Reading an INI file

\#1 Using a pointer to a `FILE` handle:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
int load_ini_file (
    FILE * ini_file,
    IniFormat format,
    IniStatsHandler f_init,
    IniDispHandler f_foreach,
    void * user_data
)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

\#2 Using a path:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
int load_ini_path (
    const char * path,
    IniFormat format,
    IniStatsHandler f_init,
    IniDispHandler f_foreach,
    void * user_data
)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

where

* `ini_file` in `load_ini_file()` is the `FILE` handle pointing to the INI file
* `path` in `load_ini_path()` is the path where the INI file is located
  (pointer to a char array, a.k.a. a "C string")
* `format` is a bitfield that defines the syntax of the INI file (see `struct`
  `IniFormat`)
* `f_init` is the function that will be invoked _before_ any dispatching begins
  -- it can be `NULL`
* `f_foreach` is the callback function that will be repeatedly invoked for each
  member of the INI file - it can be `NULL`
* `user_data` is a pointer to a custom argument -- it can be `NULL`

The user given function `f_init` (see `#IniStatsHandler` data type) will be
invoked with two arguments:

* `statistics` -- a pointer to an `IniStatistics` object containing some
  properties about the file read (like its size in bytes and the number of its
  members)
* `user_data` -- a pointer to the custom argument previously passed to the
  `load_ini_file()` / `load_ini_path()` functions

The user given function `f_foreach` (see `#IniDispHandler` data type) will be
invoked with two arguments:

* `dispatch` -- a pointer to an `IniDispatch` object containing the parsed
  member of the INI file
* `user_data` -- a pointer to the custom argument previously passed to the
  `load_ini_file()` / `load_ini_path()` functions

Both functions `load_ini_file()` and `load_ini_path()` will return zero if the
INI file has been completely dispatched, non-zero otherwise.


### Basic examples

\#1 Using a pointer to a `FILE` handle:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  examples/topics/load_ini_file.c  */

#include <stdio.h>
#include <confini.h>

static int my_callback (IniDispatch * dispatch, void * v_null) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  FILE * const ini_file = fopen("../ini_files/delivery.conf", "rb");

  if (ini_file == NULL) {

    fprintf(stderr, "File doesn't exist :-(\n");
    return 1;

  }

  if (load_ini_file(
    ini_file,
    INI_DEFAULT_FORMAT,
    NULL,
    my_callback,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  fclose(ini_file);

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

\#2 Using a path:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  examples/topics/load_ini_path.c  */

#include <stdio.h>
#include <confini.h>

static int my_callback (IniDispatch * dispatch, void * v_null) {

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  if (load_ini_path(
    "../ini_files/delivery.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    my_callback,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### How it works

The function `load_ini_path()` is a clone of `load_ini_file()` that requires a
path instead of a `FILE` handle.

Both functions `load_ini_file()` and `load_ini_path()` dynamically allocate at
once the whole INI file into the heap, and the two structures `IniStatistics`
and `IniDispatch` into the stack. All members of the INI file are then
dispatched to the custom listener `f_foreach()`. Finally the allocated memory
gets automatically freed.

Because of this mechanism _it is very important that all the dispatched data be
**immediately** copied by the user (when needed), and no pointers to the passed
data be saved_: after the end of the functions `load_ini_file()` /
`load_ini_path()` all the allocated data will be destroyed indeed -- and
moreover each dispatch might overwrite data from previous dispatches.

Within a dispatching cycle, the structure containing each dispatch
(`IniDispatch * dispatch`) is always the same `struct` that gets constantly
updated with new information.

<em><strong>Note:</strong> In some platforms, such as Microsoft Windows, it
might be needed to add the binary specifier (`"b"`) to the mode string of the
`FILE` handle passed to `load_ini_file()` in order to prevent discrepancies
between the physical size and the computed size of the file:</em>

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
FILE * ini_file = fopen("example.conf", "rb");
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## Parsing a buffer instead of a file

Starting from version 1.10.0, it is possible to parse a disposable buffer
containing an INI file instead of a physical file (i.e., to parse a `char`
array). The function that allows to do so is named `strip_ini_cache()`. This
function presents some important differences when compared to `load_ini_file()`
and `load_ini_path()`:

1. As the name suggests, the buffer passed is not left untouched, but gets
   tokenized and rearranged instead -- if you want to keep the original buffer
   you must pass a copy of it to `strip_ini_cache()` -- you can use `strndup()`
   for this, or use the example below
2. No memory is freed after the dispatching cycle is over: getting rid of the
   disposable buffer is something that must be done manually
3. The strings dispatched are all tokens from the buffer passed as argument (no
   new memory is ever allocated), but the existence of each token is granted
   only for a short time, that is _until the dispatch of the next node_ (in
   fact the latter may overwrite previous information) -- therefore, as with
   `load_ini_file()` and `load_ini_path()`, every needed information must be
   copied immediately with each dispatch
4. After the dispatching cycle is over, the buffer passed as argument must be
   regarded as _unreliable information_ (portions of it might have been
   repeatedly overwritten and corrupted by subsequent dispatches)

In short, `strip_ini_cache()` works exactly like `load_ini_file()` and
`load_ini_path()`, but with the difference that it destroys the input while it
dispatches it. And of course the input is not anymore a file, but a disposable
buffer instead. As a matter of fact, `strip_ini_cache()` is the main parsing
function both `load_ini_file()` and `load_ini_path()` rely on in order to
dispatch the content of an INI file. For a sample usage, please see
`examples/topics/strip_ini_cache.c`.

If you want to automatize the process of making a copy of a read-only buffer,
strip and parse the copy, then free the allocated memory, you can use the
following function:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  examples/utilities/load_ini_buffer.h  */

#include <stdio.h>
#include <string.h>
#include <confini.h>

int load_ini_buffer (
  const char * const ini_buffer,
  const size_t ini_length,
  const IniFormat format,
  const IniStatsHandler f_init,
  const IniDispHandler f_foreach,
  void * const user_data
) {

  char * const ini_cache = strndup(ini_buffer, ini_length);

  if (!ini_cache) {

    return CONFINI_ENOMEM;

  }

  const int retval = strip_ini_cache(
    ini_cache,
    ini_length,
    format,
    f_init,
    f_foreach,
    user_data
  );

  free(ini_cache);

  return retval;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The function above can be then invoked directly on a `const` buffer:

~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
load_ini_buffer(
  my_const_buffer,
  strlen(my_const_buffer),
  my_format,
  my_stats_handler,
  my_callback,
  my_other_data
);
~~~~~~~~~~~~~~~~~~~~~~~~~~

Since in most cases an INI buffer _is_ a disposable buffer (unless one wants to
parse the very same buffer more than once), **libconfini**'s interface does not
include the function in the example above.


## The `IniFormat` data type

For a correct use of this library it is helpful to understand the `IniFormat`
data type. **libconfini** has been born as a general INI parser, with the main
purpose of _being able to parse INI files written by other programs_ (see
**Rationale**), therefore some flexibility was required. When an INI file is
parsed it is parsed according to a particular format. The `IniFormat` data type
is a univocal description of such format. It is implemented as a 24-bit
bitfield. Its small size (3 bytes) allows it to be passed by value to the
functions that require it.

Since no function requires a pointer to an `IniFormat` data type as argument, a
preprocessor macro can be a good place where to store a custom format:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define MY_FORMAT \
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
    .implicit_is_not_empty = false, \
    .do_not_collapse_values = false, \
    .preserve_empty_quotes = false, \
    .disabled_after_space = false, \
    .disabled_can_be_implicit = false \
  })

if (load_ini_path("example.conf", MY_FORMAT, NULL, my_cb, NULL)) {

  fprintf(stderr, "Sorry, something went wrong :-(\n");
  return 1;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### The model formats

A default format named `#INI_DEFAULT_FORMAT` is available.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_default_format;

my_default_format = INI_DEFAULT_FORMAT;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The code above corresponds to:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_default_format;

my_default_format.delimiter_symbol = INI_EQUALS;  // or '='
my_default_format.case_sensitive = false;
my_default_format.semicolon_marker = INI_DISABLED_OR_COMMENT;
my_default_format.hash_marker = INI_DISABLED_OR_COMMENT;
my_default_format.section_paths = INI_ABSOLUTE_AND_RELATIVE;
my_default_format.multiline_nodes = INI_MULTILINE_EVERYWHERE;
my_default_format.no_single_quotes = false;
my_default_format.no_double_quotes = false;
my_default_format.no_spaces_in_names = false;
my_default_format.implicit_is_not_empty = false;
my_default_format.do_not_collapse_values = false;
my_default_format.preserve_empty_quotes = false;
my_default_format.disabled_after_space = false;
my_default_format.disabled_can_be_implicit = false;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

or, equivalently, in macro form:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define my_default_format \
  ((IniFormat) { \
    .delimiter_symbol = INI_EQUALS,  /*  or '='  */ \
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Starting from version 1.7.0 a format named `#INI_UNIXLIKE_FORMAT` is available.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_unixlike_format = INI_UNIXLIKE_FORMAT;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This format is a clone of `#INI_DEFAULT_FORMAT` with the only exception of
`IniFormat::delimiter_symbol`, whose value is set to `#INI_ANY_SPACE` instead
of `#INI_EQUALS`.

The semantics of the `IniFormat` bitfield has been designed in order to ensure
that when all its fields are set to zero it equals `#INI_UNIXLIKE_FORMAT`.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat format_zero = (IniFormat) { 0 };

printf(

  "`format_zero` and `INI_UNIXLIKE_FORMAT` are %s.\n",

  memcmp(&format_zero, &INI_UNIXLIKE_FORMAT, sizeof(IniFormat)) ?
    "not equal"
  :
    "equal"

); // "`format_zero` and `INI_UNIXLIKE_FORMAT` are equal."
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For further formats, please refer to the [extensible list of common formats][1]
in the project's wiki. Feel free to contribute.


## The `#IniFormatNum` data type

Each format can be represented also as a univocal 24-bit unsigned integer. In
order to convert an `IniFormat` to an unsigned integer and vice versa the
functions `ini_fton()` and `ini_ntof()` are available. For simplicity, instead
of using a `uint_least32_t` type, a size-agnostic custom type is used for this:
the `#IniFormatNum` data type.

For instance, imagine we want to create a format as close as possible to the
typical Windows INI files. Probably we would define our format as follows:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_format = {
  .delimiter_symbol = INI_EQUALS,
  .case_sensitive = false,
  .semicolon_marker = INI_ONLY_COMMENT,
  .hash_marker = INI_IS_NOT_A_MARKER,
  .section_paths = INI_ABSOLUTE_ONLY,
  .multiline_nodes = INI_NO_MULTILINE,
  .no_single_quotes = false,
  .no_double_quotes = false,
  .no_spaces_in_names = false,
  .implicit_is_not_empty = false,
  .do_not_collapse_values = false,
  .preserve_empty_quotes = false,
  .disabled_after_space = false,
  .disabled_can_be_implicit = false
};

IniFormatNum my_format_num = ini_fton(my_format);

printf("Format No. %u\n", my_format_num); // "Format No. 56637"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

According to the `ini_fton()` function this format is univocally the format
No. 56637. The function `ini_ntof()` then gives us a shortcut to construct the
very same format using its format number. Hence, the code above corresponds to:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_format = ini_ntof(56637);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## The `IniStatistics` and `IniDispatch` structures

When the functions `load_ini_file()`, `load_ini_path()` read an INI file, or
when the function `strip_ini_cache()` parses a buffer, they dispatch the file
content to the `f_foreach()` listener. Before the dispatching begins some
statistics about the parsed file can be dispatched to the `f_init()` listener
(if this is non-`NULL`).

The information passed to `f_init()` is passed through an `IniStatistics`
structure, while the information passed to `f_foreach()` is passed through an
`IniDispatch` structure.


## Rendering

The output strings dispatched by **libconfini** follow some formatting rules
depending on their role within the INI file. First, multi-line escape sequences
will be unescaped, then

* **Key names** will be rendered according to ECMAScript
  `key_name.replace(/^[\n\r]\s*|\s+/g, " ")` -- within single or double quotes,
  if active, the text will be rendered verbatim.
* **Section paths**, if format supports nesting, will be rendered according to
  ECMAScript
  `section_name.replace(/\.*\s*$|(?:\s*(\.))+\s*|^\s+/g, "$1").replace(/\s+/g,
  " ")` -- within single or double quotes, if active, the text will be rendered
  verbatim -- otherwise, will be rendered according to the same algorithm used
  for key names.
* **Values**, if `format.do_not_collapse_values` is active, will only be
  cleaned of spaces at the beginning and at the end; otherwise will be rendered
  according to the same algorithm used for key names (with the difference that,
  if `format.preserve_empty_quotes` is set to `true`, empty quotes surrounded
  by spaces will be preserved).
* **Comments**, in multi-line formats, will be rendered according to ECMAScript
  `comment_string.replace(/(^|\n\r?|\r\n?)[ \t\v\f]*[#;]+/g, "$1")`; elsewhere,
  according to ECMAScript `comment_string.replace(/^[ \t\v\f]*[#;]+/, "")`.
* **Unknown nodes** will be rendered verbatim.


## String comparisons

In order to perform comparisons between the strings dispatched the functions
`ini_string_match_ss()`, `ini_string_match_si()`, `ini_string_match_ii()` and
`ini_array_match()` are available. The function `ini_string_match_ss()`
compares two simple strings, the function `ini_string_match_si()` compares a
simple string with an unparsed INI string, the function `ini_string_match_ii()`
compares two unparsed INI strings, and the function `ini_array_match()`
compares two INI arrays. INI strings are the strings typically dispatched by
`load_ini_file()`, `load_ini_path()` or `strip_ini_cache()` which may contain
quotes and the three escape sequences `\\`, `\'`, `\"`. Simple strings are
user-given strings or the result of `ini_string_parse()`.

As a consequence, the functions `ini_string_match_si()`,
`ini_string_match_ii()` and `ini_array_match()` do not perform literal
comparisons of equality between strings. For example, in the following (absurd)
INI file the two keys `foo` and `hello` belong to the same section named `this
is a double quotation mark: "!` (after being parsed by `ini_string_parse()`).

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

  ini_string_match_ii(string1, string2, INI_DEFAULT_FORMAT) ?
    "They match"
  :
    "They don't match"
);  // "They match"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Or, for instance, in the following example the first two arrays are considered
equal, while the third one is considered different.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <stdio.h>
#include <confini.h>

int main () {

  char
    list_1[] = "foo:bar  :  apple : 'I said: wait!' :   bye bye  ",
    list_2[] = "'foo':'bar':'apple':'I said: wait!':'bye'   bye",
    list_3[] = "foo:bar:tomorrow:apple:I said: wait!:bye bye";

  printf(
    "%s\n",
    ini_array_match(list_1, list_2, ':', INI_DEFAULT_FORMAT) ?
      "They match"
    :
      "They don't match"
  );  // "They match"


  printf(
    "%s\n",
    ini_array_match(list_1, list_3, ':', INI_DEFAULT_FORMAT) ?
      "They match"
    :
      "They don't match"
  );  // "They don't match"

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In formats that support quotes, the function `ini_array_match()` is also the
function that should be used, with `'.'` or `#INI_DOT` as delimiter (see `enum`
`#IniDelimiters`), to compare properly section paths containing more than one
level of nesting.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
if (
  ini_array_match("foo.bar", disp->append_to, '.', disp->format) &&
  ini_string_match_si("username", disp->data, disp->format)
) {

  // Do something

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In case of multiple comparisons you might want to use a macro:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
static int my_callback (IniDispatch * dsp, void * v_my_data) {

  #define IS_KEY(SECTION, KEY) \
    (ini_array_match(SECTION, dsp->append_to, '.', dsp->format) && \
    ini_string_match_ii(KEY, dsp->data, dsp->format))

  if (dsp->type == INI_KEY) {

    if (IS_KEY("europe.madrid", "have_visited")) {

      // Do something

    } else if (IS_KEY("europe.london", "date")) {

      // Do something

    } else if (...) {

      // etc.

    }

  }

  #undef IS_KEY

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The four functions `ini_string_match_ss()`, `ini_string_match_si()`,
`ini_string_match_ii()`, `ini_array_match()` perform case-sensitive or
case-insensitive comparisons depending on the format given. UTF-8 codepoints
out of the ASCII range are always compared case-sensitive.

Note that within INI strings empty quotes and spaces out of quotes are always
collapsed during comparisons. Furthermore, remember that the multi-line escape
sequence `/\\(?:\n\r?|\r\n?)/` is _not_ considered as such in INI strings,
since this is the only escape sequence automatically unescaped by
**libconfini** before each dispatch.


## Editing the dispatched data

The strings dispatched, as already said, must not be freed. _Nevertheless,
before being copied or analyzed they can be edited, **with some precautions**_:

1. Be sure that your edit remains within the buffer lengths given (see
   `IniDispatch::d_len` and `IniDispatch::v_len`).
2. If you want to edit the content of `IniDispatch::data` and this contains a
   section path, the `IniDispatch::append_to` properties of its children will
   either share this buffer or will concatenate it to another buffer. Hence, if
   you edit its content -- and depending on how you edit it -- you might no
   more be able to rely on the `IniDispatch::append_to` properties of this
   node's children. You would not make any damage, the loop will continue just
   fine: so if you think you are never going to use the property
   `IniDispatch::append_to`, just do it; alternatively, use `strndup()`. If
   instead `IniDispatch::data` contains a key name or a comment, it is granted
   that no other dispatch will share this buffer, so feel free to edit it
   before it gets lost.
3. Regarding `IniDispatch::value`, if it does not represent an implicit value
   (see **§ Implicit keys**) or if `IniFormat::implicit_is_not_empty` is set to
   `false`, this buffer is never shared between dispatches, so feel free to
   edit it.
4. Regarding `IniDispatch::append_to`, this buffer is likely to be shared with
   other dispatches. Again, you would not destroy the world nor generate
   errors, but you would make the next `IniDispatch::append_to`s useless. For
   this reason **the buffer pointed by `IniDispatch::append_to` is passed as
   constant**. To unquote the path parts listed in this field please use
   `strndup()`.
5. The numerical addresses of the pointers `IniDispatch::data`,
   `IniDispatch::value` and `IniDispatch::append_to` and the fields
   `IniDispatch::type`, `IniDispatch::d_len`, `IniDispatch::v_len` and
   `IniDispatch::at_len`, are constantly reset, so feel free to use them as
   custom placeholders if you like -- but check their types:
   `IniDispatch::type` has only eight bits available and
   `IniDispatch::append_to` will always point to a `const` buffer.
6. The numerical field `IniDispatch::dispatch_id` will be checked right before
   the next dispatch, so you should not edit it. The reading happens only as
   further diagnostics: if you constantly set this field to `0` the loop will
   still end at the right moment (hopefully), but if you set it instead to a
   value equal or higher than the `IniStatistics::members` previously received
   the loop will immediately stop and a `#CONFINI_EOOR` will be thrown (see
   `enum` `#ConfiniInterruptNo`). You can think of this as a dirty way of
   writing `return !0` at the end of your listener. Unless you really know what
   you are doing, do not edit `IniDispatch::dispatch_id`.

Typical peaceful edits are the ones obtained by calling the functions
`ini_array_collapse()` and `ini_string_parse()` directly on the buffer
`IniDispatch::value`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  examples/topics/ini_string_parse.c  */

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * dispatch, void * v_null) {

  if (
    dispatch->type == INI_KEY || dispatch->type == INI_DISABLED_KEY
  ) {

    ini_unquote(dispatch->data, dispatch->format);
    ini_string_parse(dispatch->value, dispatch->format);

  }

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data,
    dispatch->value,
    dispatch->type
  );

  return 0;

}

int main () {

  if (load_ini_path(
    "../ini_files/self_explaining.conf",
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If all these rules, although thoroughly exposed, sound still confusing to you,
use always `strndup()` on the strings dispatched and feel free to edit your own
buffers as you wish. Under `examples/utilities/clone_ini_dispatch.h` you can
find a function designed to make a hard copy of an entire `IniDispatch`,
including all the strings that this points to.


### Formatting the values

Once your listener starts to receive the parsed data you may want to format and
better parse the `value` part of key-value elements. The following functions
might be useful for this purpose:

* `ini_get_bool()`
* `ini_get_bool_i()`
* `ini_string_parse()`
* `ini_array_get_length()`
* `ini_array_foreach()`
* `ini_array_collapse()`
* `ini_array_break()`
* `ini_array_release()`
* `ini_array_shift()`
* `ini_array_split()`

Together with the functions listed above the following links are available, in
case you don't have `#include <stdlib.h>` in your source:

* `ini_get_int()` = [<strong>`atoi()`</strong>][2]
* `ini_get_lint()` = [<strong>`atol()`</strong>][3]
* `ini_get_llint()` = [<strong>`atoll()`</strong>][4]
* `ini_get_double()` = [<strong>`atof()`</strong>][5]

Further useful resources include:

* `examples/utilities/clone_ini_dispatch.h`
* `examples/utilities/make_strarray.h`


### Formatting the key names

The function `ini_unquote()` can be useful for key names enclosed within
quotes. This function is very similar to `ini_string_parse()`, except that does
not bother collapsing the sequences of more than one space that might result
from removing empty quotes -- this is never necessary, since empty quotes
surrounded by spaces in key and section names are always automatically
collapsed before being dispatched.

You could use `ini_string_parse()` as well to parse key and section names, but
you would obtain the same result with a slightly bigger effort from the CPU.


### Formatting the section paths

In order to retrieve the parts of a section path, the functions
`ini_array_get_length()`, `ini_array_foreach()`, `ini_array_break()`,
`ini_array_release()`, `ini_array_shift()` and `ini_array_split()` can be used
with `'.'` or `#INI_DOT` as delimiter (see `enum` `#IniDelimiters`). Note that
section paths dispatched by **libconfini** are always collapsed arrays,
therefore calling the function `ini_array_collapse()` on them will have no
effects.

It might be required that the function `ini_unquote()` be applied to each part
of a section path, depending on the content and the format of the INI file.


## Implicit keys

In order to set the value to assign to implicit keys (i.e. keys without a
delimiter and a value), please use the `ini_global_set_implicit_value()`
function. A `true` boolean is usually a good choice:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
ini_global_set_implicit_value("YES", 3);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alternatively, instead of `ini_global_set_implicit_value()` you can manually
declare at the beginning of your code the two global variables
`#INI_GLOBAL_IMPLICIT_VALUE` and `#INI_GLOBAL_IMPLICIT_V_LEN`, which will be
retrieved by **libconfini**:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <confini.h>

char * INI_GLOBAL_IMPLICIT_VALUE = "YES";
size_t INI_GLOBAL_IMPLICIT_V_LEN = 3;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Or you can assign a value to them at the beginning of the `main()` function of
your program:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <confini.h>

int main () {

  INI_GLOBAL_IMPLICIT_VALUE = "YES";
  INI_GLOBAL_IMPLICIT_V_LEN = 3;

  /* ... */

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If not defined elsewhere, these variables are initialized respectively to
`NULL` and `0` by default.

Although the two variables `#INI_GLOBAL_IMPLICIT_VALUE` and
`#INI_GLOBAL_IMPLICIT_V_LEN` are used only as placeholders for custom
information accessible solely by the user, starting from version 1.14.0 it is
safer to make `#INI_GLOBAL_IMPLICIT_V_LEN` match _exactly_ the real length of
`#INI_GLOBAL_IMPLICIT_VALUE` (without counting the `NUL` terminator). By doing
so it is possible to make **libconfini** aware of a segment of memory that must
be protected from writing operations.

After having set the value to be assigned to implicit key elements, and having
enabled `IniFormat::implicit_is_not_empty` in the format, it is possible to
test whether a dispatched key is implicit or not by comparing the address of
its `value` property with the global variable `#INI_GLOBAL_IMPLICIT_VALUE`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  examples/topics/ini_global_set_implicit_value.c  */

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * dispatch, void * v_null) {

  if (dispatch->value == INI_GLOBAL_IMPLICIT_VALUE) {

    printf(
      "\nDATA: %s\nVALUE: %s\nNODE TYPE: %u\n"
      "(This is an implicit key element)\n",

      dispatch->data,
      dispatch->value,
      dispatch->type
    );

  } else {

    printf(
      "\nDATA: %s\nVALUE: %s\nNODE TYPE: %u\n",

      dispatch->data,
      dispatch->value,
      dispatch->type
    );

  }

  return 0;

}

int main () {

  IniFormat my_format = INI_UNIXLIKE_FORMAT;

  ini_global_set_implicit_value("[implicit default value]", 24);

  /*  Without this implicit keys will be considered empty  */
  my_format.implicit_is_not_empty = true;
  my_format.disabled_can_be_implicit = true;

  if (load_ini_path(
    "../ini_files/unix-like.conf",
    my_format,
    NULL,
    ini_listener,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Implicit keys can be parsed as booleans also without setting
`IniFormat::implicit_is_not_empty` to `true`. By doing so there will be no
distinction between empty and implicit keys, and there are situations where
this can be a wanted behavior. The following example will parse both `my_key`
and `my_key =` in the INI file as `true`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  `dsp->format.implicit_is_not_empty` is `false` here!  */
bool my_boolean;
if (ini_string_match_si("my_key", dsp->data, dsp->format)) {
  my_boolean = (bool) ini_get_bool_i(dsp->value, 1, dsp->format);
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


## Code considerations


### Return values

The functions `load_ini_file()`, `load_ini_path()`, `strip_ini_cache()`,
`ini_array_foreach()` and `ini_array_split()` require some listeners defined by
the user. Such listeners must return an `int` value. When this is non-zero the
caller function is interrupted, its loop stopped, and a non-zero value is
returned by the caller as well.

The functions `load_ini_file()`, `load_ini_path()` and `strip_ini_cache()`
return a non-zero value also if the INI file, for any reason, has not been
completely parsed (see `enum` `#ConfiniInterruptNo`). Therefore, in order to be
able to distinguish between internal errors and user-generated interruptions
the mask `#CONFINI_ERROR` can be used.

For instance, in the following example the `f_foreach()` listener returns a
non-zero value if a key named `password` with a value that equals `Hello world`
is found. Hence, by using the mask `#CONFINI_ERROR`, the code below
distinguishes a non-zero value generated by the listener from a non-zero value
due to a parsing error.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  examples/topics/ini_string_match_si.c  */

#include <stdio.h>
#include <confini.h>

static int passfinder (IniDispatch * disp, void * v_membid) {

  /*  Search for `password = "hello world"` in the INI file  */
  if (
    ini_string_match_si("password", disp->data, disp->format) &&
    ini_string_match_si("hello world", disp->value, disp->format)
  ) {

    *((size_t *) v_membid) = disp->dispatch_id;
    return 1;

  }

  return 0;

}

int main () {

  size_t membid;

  /*  Load INI file  */
  int retval = load_ini_path(
    "../ini_files/self_explaining.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    passfinder,
    &membid
  );

  /*  Check for errors  */
  if (retval & CONFINI_ERROR) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  /*  Check if parsing has been interrupted by `passfinder()`  */
  retval  ==  CONFINI_FEINTR ?
                printf(
                  "We found it! It's the INI node number %zu!\n",
                  membid
                )
              :
                printf("We didn't find it :-(\n");

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### The formatting functions

The functions `ini_unquote()`, `ini_string_parse()`, `ini_array_collapse()`,
`ini_array_break()`, `ini_array_release()` and `ini_array_split()` change the
content of the given strings. It is important to point out that the edit is
always performed within the lengths of the strings given.

The behavior of these functions depends on the format used. In particular,
using `ini_string_parse()` as a model one obtains the following scheme:

1. Condition: `!format.no_single_quotes && !format.no_double_quotes &&
   format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`, `\"`, `\'`<br />
   ⇒ Behavior of `ini_string_parse()`: `\\`, `\'` and `\"` will be unescaped,
   all unescaped single and double quotes will be removed, then the new length
   of the string will be returned.
2. Condition: `!format.no_single_quotes && format.no_double_quotes &&
   format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`, `\'`<br />
   ⇒ Behavior of `ini_string_parse()`: `\\` and `\'` will be unescaped, all
   unescaped single quotes will be removed, then the new length of the string
   will be returned.
3. Condition: `format.no_single_quotes && !format.no_double_quotes &&
   format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`, `\"`<br />
   ⇒ Behavior of `ini_string_parse()`: `\\` and `\"` will be unescaped, all
   unescaped double quotes will be removed, then the new length of the string
   will be returned.
4. Condition: `format.no_single_quotes && format.no_double_quotes &&
   format.multiline_nodes != INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: `\\`<br />
   ⇒ Behavior of `ini_string_parse()`: only `\\` will be unescaped, spaces at
   the beginning and at the end of the string will be removed, then the new
   length of the string will be returned.
5. Condition: `format.no_single_quotes && format.no_double_quotes &&
   format.multiline_nodes == INI_NO_MULTILINE`<br />
   ⇒ Escape sequences: No escape sequences<br />
   ⇒ Behavior of `ini_string_parse()`: Spaces at the beginning and at the end
   of the string will be removed, then the new length of the string will be
   returned.

A function-like macro named `#INIFORMAT_HAS_NO_ESC()` is available in order to
check whether a format supports escape sequences or not.

It is possible to extend the list of supported escape sequences by parsing
additional ones before invoking `ini_string_parse()`. Under
`examples/utilities/ini_string_preparse.h` there is a little helper function
that adds support to the following sequences: `"\a"`, `"\b"`, `"\f"`, `"\n"`,
`"\r"`, `"\t"`, `"\v"` and `"\e"`. It must be used right before `ini_unquote()`
or `ini_string_parse()`, since it relies on the latter for unescaping double
backslashes:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <confini.h>
#include "examples/utilities/ini_string_preparse.h"

static int my_callback (IniDispatch * disp, void * user_data) {

  if (disp->type == INI_KEY) {

    /*  Add support for \a, \b, \f, \n, \r, \t, \v and \e  */
    ini_string_preparse(disp->value);
    ini_string_parse(disp->value, disp->format);

    /*  DO SOMETHING  */

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Common mistakes

The native formatting functions `ini_array_break()`, `ini_array_collapse()`,
`ini_array_release()`, `ini_array_split()`, `ini_string_parse()` and
`ini_unquote()` have a special safeguard against attempting to edit the global
variable `#INI_GLOBAL_IMPLICIT_VALUE`. But if you use your own functions for
editing the dispatches received, you must always make sure that
`dispatch->value != INI_GLOBAL_IMPLICIT_VALUE` in formats where
`IniFormat::implicit_is_not_empty` is set to `true` -- otherwise a
“Segmentation fault (core dumped)” error can be generated.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <stdio.h>
#include <confini.h>

static void my_editing_function (char * const my_string) {

  if (!my_string) {

    return;

  }

  /*  Only a random edit...  */
  my_string[0] = '\0';

}

static int my_callback (IniDispatch * dispatch, void * v_null) {

  /*
    This will always work: `libconfini` formatting function have a
    safeguard against attempting to edit the global variable
    `INI_GLOBAL_IMPLICIT_VALUE`...
  */
  ini_string_parse(dispatch->value, dispatch->format);

  /*
    This can cause a `Segmentation fault (core dumped)` error when
    `dispatch->format.implicit_is_not_empty` is set to `true` and
    `dispatch->value` is an implicit boolean. In cases like this, if
    you don't want to `strndup(dispatch->value, dispatch->v_len)`,
    you must always check that
   `dispatch->value != INI_GLOBAL_IMPLICIT_VALUE`!
  */
  my_editing_function(dispatch->value);

  printf(
    "DATA: %s\nVALUE: %s\nNODE TYPE: %u\n\n",
    dispatch->data, dispatch->value, dispatch->type
  );

  return 0;

}

int main () {

  ini_global_set_implicit_value("YES", 3);
  IniFormat myformat = INI_DEFAULT_FORMAT;
  myformat.implicit_is_not_empty = true;
  myformat.disabled_can_be_implicit = true;

  if (load_ini_path(
    "examples/ini_files/typed_ini.conf",
    myformat,
    NULL,
    my_callback,
    NULL
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To check whether a particular substring belongs to
`#INI_GLOBAL_IMPLICIT_VALUE`, the `INI_IS_IMPLICIT_SUBSTR()` macro can be used:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
if (dispatch->v_len > 0) {

  /*  Let us create a substring of `dispatch->value`...  */
  char * my_substring = dispatch->value + 1;

  printf("%s\n", my_substring);

  if (INI_IS_IMPLICIT_SUBSTR(my_substring)) {

    printf("\n\tYou tricky one! This was an implicit value too!\n");

  }

return 0;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Storing the dispatched data

In order to be as flexible as possible, **libconfini** does not store the
dispatched data, nor indicizes them. This gives the developer the power to deal
with them in many different ways.

For small INI files a normal if/else chain, using `ini_array_match()` for
comparing section paths and `ini_string_match_si()`/`ini_string_match_ii()` for
comparing key names, usually represents the most practical way to obtain the
information required from an INI file. Sometimes however, especially in case of
sizeable INI files, the most efficient solution would be to store the parsed
data in a hash table before trying to access it.

Some INI parsers are released with a hash table API included by default. This
is often an unpractical solution, since fantastic free software libraries that
focus solely on hash tables already exist, and providing a further API for
managing a hash function together with an INI parser only complicates the code,
makes it harder to maintain, and does not give the user the real freedom to
choose what suits best to each single case. Some programming languages have
even hash tables in their standard libraries (see `std::map` in C++ for
example).

When needed, the data parsed by **libconfini** can still be stored in a hash
table while it is being dispatched. If you are interested in combining
**libconfini** with a hash table, I have left a general example of how to use
**GLib**'s `GHashTable` together with **libconfini** under
`examples/miscellanea/glib_hash_table.c`. If you are using C++, you can find an
example of how to construct a C++ class that relies on a `std::unordered_map`
object under `examples/cplusplus/map.cpp`. By keeping these examples as models
other solutions can be easily explored as well.


### Size of the dispatched data

Writing

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
char * buf  =   malloc(stats->bytes + stats->members + (
                  stats->format.implicit_is_not_empty ?
                    INI_GLOBAL_IMPLICIT_V_LEN * stats->members
                  :
                    0
                ) + 1);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

will always allocate the smallest buffer large enough to store all the
`IniDispatch::data` and `IniDispatch::value` received, including their `NUL`
terminators and any possible implicit value.

This buffer will not be capable enough to store also the
`IniDispatch::append_to` strings received (about which no information is
available at early stages). However, thanks to the syntax of INI files, where a
key is always appended to the section previously dispatched, it will be always
possible to store the information about the structure of the tree by using
numbers as references to the parents instead of copying verbatim the
`IniDispatch::append_to` strings. This will also result in a more efficient
code.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include <stdio.h>
#include <stdlib.h>
#include <confini.h>

static int my_stats_listener (
  IniStatistics * stats,
  void * v_destptr
) {

  #define destptr ((char **) v_destptr)

  *destptr  =   malloc(stats->bytes + stats->members + (
                  stats->format.implicit_is_not_empty ?
                    INI_GLOBAL_IMPLICIT_V_LEN * stats->members
                  :
                    0
                ) + 1);

  return 0;

  #undef destptr

}


static int my_ini_listener (
  IniDispatch * this,
  void * v_my_capable_buffer
) {

  /*  Store the dispatched data in `my_capable_buffer`  */
  char ** my_capable_buffer = (char **) v_my_capable_buffer;
  /*  ...  */

  return 0;

}

int main () {

  /*  In real life we would need a more complex `struct` here...  */
  char * my_copied_data;

  /*  Load the INI file  */
  if (load_ini_path(
    "test.conf",
    INI_DEFAULT_FORMAT,
    my_stats_listener,
    my_ini_listener,
    &my_copied_data
  )) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Other global settings

In the past, besides the two global variables `#INI_GLOBAL_IMPLICIT_VALUE` and
`#INI_GLOBAL_IMPLICIT_V_LEN`, a third variable named
`#INI_GLOBAL_LOWERCASE_MODE` would tell **libconfini** whether to dispatch in
lower case all key names and section paths of case-insensitive INI files.
This variable rarely needed to be set to `true`, since string comparisons made
by libconfini are always either case-sensitive or case-insensitive depending on
the format given.

This “dispatch in lowercase” functionality itself was a relic from a time when
**libconfini** did not yet possess any string comparison functions. However, as
this is no longer the case and there are now four functions for that, starting
from version 1.15.0 both the `#INI_GLOBAL_IMPLICIT_VALUE` variable and the
`ini_global_set_lowercase_mode()` function have been marked as deprecated (see
the **list of deprecated functions and variables**). If needed, to convert to
lowercase key names and section paths in case-insensitive INI files it is
possible to use the following simple snippet, which maps exactly what
**libconfini** would do:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
static inline void string_tolower (char * const str) {
  for (register char * ptr = str; *ptr; ptr++) {
    *ptr = *ptr > 0x40 && *ptr < 0x5b ? *ptr | 0x60 : *ptr;
  }
}

static int my_callback (IniDispatch * dispatch, void * v_null) {

  if (!dispatch->format.case_sensitive) {
    switch (dispatch->type) {
      case INI_KEY:
      case INI_DISABLED_KEY:
      case INI_SECTION:
      case INI_DISABLED_SECTION:
        string_tolower(dispatch->data);
    }

  }

  /*  DO SOMETHING...  */

 return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### The unicode problem

Comparing an ASCII upper case letter to an ASCII lower case letter is an
invariant process. But comparing two Unicode letter cases is a process that
depends on the locale of the machine. Consider for example the lower case
letter `i`: in most European languages its upper case is `I`, while this is not
the case in Turkish, where the upper case of `i` is `İ` (and the lower case of
`I` is `ı`). Therefore for a person living in Italy or France, `i` and `I` will
represent the same letter, while for a person living in Turkey they will not.

Key and section names of an INI file however cannot depend on the locale of the
machine, since they must be reliably searched for independently of where a
machine is located. Imagine for example a key named “INI” and imagine that
Unicode case folding were performed on key names during string comparisons. If
you lived in Europe you could look up for such key using its lower case “ini”,
while if you lived in Turkey you would have to use the lower case “ını” to find
it. So the only solution in this context is to consider Unicode characters out
of the ASCII range always as case-sensitive. For this reason, **libconfini**
(and probably any senseful INI parser) will never perform case folding of
Unicode characters out of the ASCII range within key and section names. 

It must be said however that most Unicode characters do not possess a lower and
upper case, and most characters outside of the ASCII range could theoretically
appear without problems in key and section names also in case-insensitive INI
files (think of the character `§` for example). And, as for case-sensitive INI
files, no Unicode character would ever represent a problem. Nonetheless, it is
still generally more acceptable to use ASCII only within key and section names
-- and possibly, if needed, non-ASCII Unicode characters within values and
comments.

That said, **libconfini** deals perfectly fine with UTF-8 (but is always
case-sensitive outside of the ASCII range), so use the latter as you feel
appropriate.


### Thread safety

Depending on the format of the INI file, **libconfini** may use up to three
global variables (`#INI_GLOBAL_IMPLICIT_VALUE`, `#INI_GLOBAL_IMPLICIT_V_LEN`
and `#INI_GLOBAL_LOWERCASE_MODE`). In order to be thread-safe these three
variables (if needed) must be defined only once (either directly, or by using
their setter functions `ini_global_set_implicit_value()` and
`ini_global_set_lowercase_mode()`), or otherwise a mutex logic must be
introduced.

Apart from the three variables above, each parsing allocates and frees its own
memory and every function is fully reentrant, therefore the library can be
considered thread-safe.


### Error exceptions

The philosophy of **libconfini** is to parse as much as possible without
generating error exceptions. No parsing errors are returned once an INI file
has been correctly allocated into the heap, with the exception of the
_out-of-range_ error `#CONFINI_EOOR` (see `enum` `#ConfiniInterruptNo`), whose
meaning is that the dispatches are for unknown reasons more than expected --
this error is possibly generated by the presence of bugs in the library's code
and should **never** be returned (please [contact me][6] if this happens).

When an INI node is wrongly written in respect to the format given, it is
dispatched verbatim as an `#INI_UNKNOWN` node -- see `enum` `#IniNodeType`.
Empty lines, or lines containing only spaces and empty quotes (if the latter
are supported) will be skipped.

In order to avoid error exceptions, strings containing an unterminated quote
will be always treated as if they had a virtual quote as their last + 1
character. For example,

~~~~~~~~~~{.ini}
foo = "bar
~~~~~~~~~~

will always determine the same behavior as if it were

~~~~~~~~~~~{.ini}
foo = "bar"
~~~~~~~~~~~

Any format containing the following three settings will never produce
`#INI_UNKNOWN` nodes, even if instead of an INI file we tried to parse a .jpeg
image (or anything else):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
IniFormat my_format = {
  .delimiter_symbol = INI_ANY_SPACE,
  .section_paths = INI_NO_SECTIONS,
  .no_spaces_in_names = false,
  ...
};
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Performance

The algorithms used by **libconfini** stand in a delicate equilibrium between
flexibility, speed and code readability, with flexibility as primary target.
Performance can vary with the format used to parse an INI file, but in most
cases is not a concern.

One can measure the performance of the library by doing something like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/*  dev/tests/performance/performance.c  */

#include <stdio.h>
#include <confini.h>
#include <time.h>

static int get_ini_size (IniStatistics * stats, void * v_bytes) {

  *((size_t *) v_bytes) = stats->bytes;
  return 0;

}

static int empty_listener (IniDispatch * dispatch, void * v_bytes) {

  return 0;

}

int main () {

  size_t bytes;
  double seconds;

  clock_t start, end;
  start = clock();

  /*  Please create an INI file large enough  */
  if (load_ini_path(
    "big_file.ini",
    INI_DEFAULT_FORMAT,
    get_ini_size,
    empty_listener,
    &bytes
  )) {

    return 1;

  }

  end = clock();
  seconds = (double) (end - start) / CLOCKS_PER_SEC;

  printf(
    "%zu bytes parsed in %f seconds.\n"
    "Number of bytes parsed per second: %f\n",
    bytes, seconds, (double) bytes / seconds
  );

  return 0;

}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By changing the format of the INI file on the code above you may obtain
different results. In particular, switching disabled entry recognition off --
by setting `IniFormat::semicolon_marker` and `IniFormat::hash_marker` to
`#INI_ONLY_COMMENT` or `#INI_IGNORE` -- and making the format non-multi-line --
by setting `IniFormat::multiline_nodes` to `#INI_NO_MULTILINE` -- will have a
positive impact on the performance.

On my laptop **libconfini** seems to parse around 95 MiB per second using the
model format `#INI_DEFAULT_FORMAT`. Whether this is enough for you or not, that
depends on your needs.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
53411840 bytes (1146880 nodes) parsed in 0.533315 seconds.
Number of bytes parsed per second: 100150642.678342
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you are interested in testing yourself the library's performance on a
particular hardware you can find a performance test under
`dev/tests/performance`.


## INI syntax considerations


### Comment or disabled entry?

I can hardly imagine a reason to be interested in disabled entries if not for
writing a GUI editor for INI files. However, if this is the case and you are
not using **libconfini** like normal people do, you might wonder how to ensure
that disabled entries and comments be always parsed without ambiguity.

In most of the cases **libconfini** is smart enough to distinguish a disabled
entry from a comment. However some INI files can be tricky and might require
some workarounds. For instance, imagine to have the following INI file:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
# INI KEY/VALUE DELIMITER: `=`

[some_section]

hello = world

;foo = bar

##now=Sunday April 3rd, 2016
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

And imagine that for unknown reasons the author of the INI file wanted only
`;foo = bar` to be considered as a disabled entry, and the first and last line
as normal comments.

If we tried to parse it according to the format used below

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
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
      .semicolon_marker = INI_IGNORE, \
      .hash_marker = INI_IS_NOT_A_MARKER, \
      .multiline_nodes = INI_NO_MULTILINE, \
      .section_paths = INI_ABSOLUTE_ONLY, \
      .no_single_quotes = false, \
      .no_double_quotes = false, \
      .no_spaces_in_names = false, \
      .implicit_is_not_empty = true, \
      .do_not_collapse_values = false, \
      .preserve_empty_quotes = false, \
      .disabled_after_space = true, \
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

we would obtain the following result:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:: Content of "ambiguous.conf" ::

#0 - Type: 2; Data: "# INI KEY/VALUE DELIMITER: `"; Value: "`"
#1 - Type: 3; Data: "some_section"; Value: ""
#2 - Type: 2; Data: "hello"; Value: "world"
#3 - Type: 2; Data: "##now"; Value: "Sunday April 3rd, 2016"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As you can see, all comments but `now=Sunday April 3rd, 2016` would be parsed
as disabled entries -- which is not what the author intended. Therefore, to
ensure that such INI file be parsed properly, you can follow two possible
approaches.

**1. Intervene on the INI file.** The reason why `now=Sunday April 3rd, 2016`
has been properly parsed as a comment -- despite it really looks like a
disabled entry -- is because it has been nested within a comment block opened
by more than one leading marker (in this case the two `##`). As a general rule,
_**libconfini** never parses a comment beginning with more than one leading
marker as a disabled entry_, therefore this is the surest way to ensure that
proper comments are always considered as such.

Hence, by adding one more number sign to the first comment

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}
## INI KEY/VALUE DELIMITER: `=`

[some_section]

hello = world

;foo = bar

##now=Sunday April 3rd, 2016
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

we obtain the wanted result:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:: Content of "ambiguous.conf" ::

#0 - Type: 4; Data: " INI KEY/VALUE DELIMITER: `=`"; Value: ""
#1 - Type: 3; Data: "some_section"; Value: ""
#2 - Type: 2; Data: "hello"; Value: "world"
#3 - Type: 6; Data: "foo"; Value: "bar"
#4 - Type: 4; Data: "now=Sunday April 3rd, 2016"; Value: ""
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**2. Intervene on the format.** There are cases where the INI file is
automatically generated by machines (comments included), or distributed as
such, and human intervention would be required on each machine-generated
realease of the INI file. In these cases -- and if we are sure about the
expected content of the INI file -- we can restrict the format chosen in order
to parse comments and disabled entries properly. In particular, the following
fields of the `IniFormat` bitfield can have an impact on the disambiguation
between comments and disabled entries.

Reliable general patterns:

* `IniFormat::semicolon_marker` and `IniFormat::hash_marker` -- The imaginary
  author of our INI file, if we observe the latter closer, chose the semicolon
  symbol as the marker of disabled entries and the hash symbol as the marker of
  comments. We may exploit this difference and set our
  `my_format.semicolon_marker` to `#INI_DISABLED_OR_COMMENT` and our
  `my_format.hash_marker` to `#INI_ONLY_COMMENT` to obtain the correct
  disambiguation. If you believe that this solution is too artificial, think
  that `/etc/samba/smb.conf` and `/etc/pulse/daemon.conf` are systematically
  distributed using this pattern.
* `IniFormat::disabled_after_space` -- Setting this property to `false`, due to
  the initial space that follows the comment marker (`# INI...`), forces the
  entire line to be considered as a comment. Some authors use this syntax to
  distinguish between comments and disabled entries (examples are
  `/etc/pacman.conf` and `/etc/bluetooth/main.conf`)

Temporary workarounds:

* `IniFormat::no_spaces_in_names` -- If our INI file has only comments
  containing more than one word and we are sure that key and section names
  cannot contain internal white spaces, we can set this property to `true` to
  enhance disambiguation.
* `IniFormat::disabled_can_be_implicit` -- This property, if set to `false`,
  forces all comments that do not contain a key-value delimiter never to be
  considered as disabled entries. Despite not having an impact on our example,
  it has an impact on the disambiguation algorithms used by **libconfini**. Its
  value in `#INI_DEFAULT_FORMAT` is set to `false`.

As a general rule, **libconfini** will always try to parse as a disabled entry
whatever comment is allowed (by the format) to contain one. Only if this
attempt fails, the block will be dispatched as a normal comment.


[1]: https://github.com/madmurphy/libconfini/wiki/INI-formats
[2]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoi
[3]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atol
[4]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atoll
[5]: http://www.gnu.org/software/libc/manual/html_node/Parsing-of-Integers.html#index-atof
[6]: https://github.com/madmurphy/libconfini/issues

