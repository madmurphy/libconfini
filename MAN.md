Library Functions Manual {#libconfini}
======================================

@author Stefano Gioffr&eacute;

# OVERVIEW

**libconfini** is a simple INI parsing library with the ability to read disabled entries (i.e., commented lines). **libconfini** does not store the data read from an INI file, but rather dispatches it, formatted, to a custom listener.

The code does not depend on any particular library, except for the C standard libraries **stdio.h** and **stdlib.h**.

If you want to start to learn directly from the code, you can find partially self-documented sample usages of **libconfini** under `/usr/share/doc/libconfini/examples/`.

## WHAT IS AN INI FILE

INI files were introduced with the early versions of Microsoft Windows, where the .ini file name extension stood for INItialization. An INI file can be considered as a string representation of a tree object, with new lines used as delimiters between nodes. A typical INI file is a plain text file looking like the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

# delivery.conf

; general options

location = Colosseum
place = Rome

[sender]

name = John Smith
email = john.smith@example.com

[receiver]

name = Mario Rossi		# He's a fat guy
email = mario.rossi@example.com

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## SUPPORTED SYNTAX

During the years, several interpretations of INI files appeared. In some implementation the colon character (`:`) has been adopted as delimiter (a typical example under GNU/Linux is `/etc/nsswitch.conf`), in other implementation the space (`/[ \t\r\v\f]+/`) has been used instead (for example in `/etc/host.conf`), and so on. This library has been born as a general INI parser for GNU, so the support of most of the main _INI dialects_ has been implemented within it.

Especially in Microsoft Windows, a more radical syntax variation has been implemented: the use of the semicolon, instead of new lines, as node delimiter, as in the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

# delivery.conf

; general options

location = Colosseum; place = Rome; [sender] name = John Smith;
email = john.smith@example.com; [receiver] name = Mario Rossi; # He's a fat guy
email = mario.rossi@example.com

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For several reasons the use of semicolon as node delimiter is not (and will never be) supported by **libconfini**.

The encodings currently supported by **libconfini** are ASCII and UTF-8 (without BOM). In case the INI file is case-insensitive with respect to keys and section names, **libconfini** will convert all ASCII letters to lowercase (except within values), but will **not** convert non-ASCII code points to lowercase. _In general it is a good practice to use UTF-8 within values, but ASCII only within keys names and sections names._

### KEYS

A **key element** is identified as a string followed by a delimiter -- typically the equals sign (`=`) or the colon sign (`:`) or a space sequence (`/[ \t\v\f]+/`) -- which is followed by a value, which is followed by a new line or an inline comment. 

Both the **key part** and the **value part** may be sorrounded by quotes (`'` or `"`):

~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

foo = "bar"
"hello" = world
"artist" = "Pablo Picasso"

~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to avoid error exceptions, strings containing an unterminated quote will always be treated as if they had a virtual quote as their last + 1 character.

~~~~~~~~~~{.ini}

foo = "bar

~~~~~~~~~~

will always behave the same as if it were

~~~~~~~~~~~{.ini}

foo = "bar"

~~~~~~~~~~~

The **key part** can contain any character, except the delimiter (which can be enclosed within quotes for not beeing considered as such). The new line sequences (`\r`, `\n`, `\r\n`, `\n\r`) must be escaped.

If the **key part** part is missing the element is considered of unknown type (example: `= foo`). If the **value part** is missing the key element is considered empty (example: `foo =`). If the delimiter is missing the key can be considered an "implicit key", typically representing the boolean `TRUE` (example: `foo`). For instance, in the following example from `/etc/pacman.conf`, `IgnorePkg` is an empty key, while `Color` is an implicit key (representing a `TRUE` boolean -- i.e., `Color = YES`):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

HoldPkg = pacman glibc
Architecture = auto
IgnorePkg =
Color
SigLevel = Required DatabaseOptional
LocalFileSigLevel = Optional

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The **value** part can contain typed data, usually: a boolean (booleans supported by **libconfini** are: `NO`/`YES`, `FALSE`/`TRUE`, `0`/`1`), a string, a number, or an array (typically with commas as members delimiters -- example: `paths = /etc, /usr, "/home/john/Personal Data"`). The new line sequences (`\r`, `\n`, `\r\n`, `\n\r`) must be escaped.

In order to set the value to be assigned to implicit keys, please use the `ini_set_implicit_value()` function. A 'zero-length `TRUE`-boolean' is usually a good choice:

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

If not defined elsewhere, these variables are respectively set to `NULL` and `0` by default.

### SECTIONS

A **section** can ben imagined like a directory. A **section path** is identified as the string `"$1"` in the regular expression (ECMAScript syntax) `/(?:^|\n)[ \t\v\f]*\[([^\]]*)\]/` globally applied to an INI file. A section path expresses nesting through the 'dot' character, as in the following example:

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

Keys appearing before any section path belong to a virtual _root_ node, like the key `foo` in the following example:

~~~~~~~~~~~~~~~~~~~{.ini}

foo = bar

[options]

interval = 3600

[host]

address = 127.0.0.1
port = 80

~~~~~~~~~~~~~~~~~~~

Section parts can be enclosed within quotes:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

["world".europe.'germany'.berlin]

foo = bar

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### COMMENTS

Comments are string segments enclosed within the sequence `/\s+[;#]/` and a new line sequence, as in the following example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

# this is a comment

foo = bar       # this is an inline comment

; this is another comment

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### ESCAPING SEQUENCES

For maximizing the flexibility of the data, only three escaping sequences are supported by **libconfini**: `\'`, `\"` and `\\`.

Escaping sequences are left untouched by all functions except `ini_unquote()`. Nevertheless, the characters `'`, `"` and `\` can determine different behaviors during the parsing depending on whether they are escaped or unescaped. For instance, the string `#johnsmith` in the following example will not be parsed as a comment:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

[users.jsmith]

comment = "hey! have a look at my hashtag #johnsmith !"

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# READ AN INI FILE

The syntax of **libconfini**'s main function is:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
unsigned int load_ini_file (
	const char *path,
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

* `path` is the path where the INI file is located (pointer to a char array, a.k.a. a "C string")
* `format` is a bitfield structure defining the syntax of the INI file (see the @ref #IniFormat object)
* `f_init` is the function that will be invoked _before_ any dispatching begins; it can be `NULL`
* `f_foreach` is the callback function that will be invoked for each member of the INI file; it can be `NULL`
* `user_data` is a pointer to a custom argument; it can be `NULL`

The function `f_init()` will be invoked with two arguments:

* `statistics` -- a pointer to a @ref #IniStatistics object containing some properties about the file read
  (like its size in bytes and the number of its members)
* `user_data` -- a pointer to the custom argument previously passed to the `load_ini_file()` function

The function `f_foreach()` will be invoked with two arguments:

* `dispatch` -- a pointer to a @ref #IniDispatch object containing the parsed member of the INI file
* `user_data` -- a pointer to the custom argument previously passed to the `load_ini_file()` function

### HOW IT WORKS

The function `load_ini_file()` dynamically allocates at once the whole INI file into the heap, and the two structures `IniStatistics` and `IniDispatch` into the stack. All the members of the INI file are then dispatched to the listener `f_foreach()`. Finally the allocated memory gets automatically freed.

Because of this mechanism _it is very important the all the dispatched data be **immediately** copied by the user, if needed, and no pointers to the passed data be saved_: after the end of the `load_ini_file()` function all the allocated data will be destroyed indeed.

Within a dispatching cycle, the structure containing each dispatch (`IniDispatch *dispatch`) is always the same `struct` that gets constantly updated with new information.

The strings passed with each dispatch must therefore not be freed. _Nevertheless, before being copied or analyzed they can be edited, **with some precautions**_:

1. Be sure that your edit remains within the buffer lengths given (see: `IniDispatch::d_len` and `IniDispatch::v_len`).
2. If you want to edit the content of `IniDispatch::data` and this contains a section path, the `IniDispatch::append_to` properties of its children _may_ refer to the same buffer: if you edit it you can no more rely on its children's `IniDispatch::append_to` properties (you will not make any damage, the loop will continue just fine: so if you think you are going to never use the property `IniDispatch::append_to` just do it).
3. Regarding `IniDispatch::value`, the buffer will not be shared between dispatches, so feel free to edit it.
4. Regarding `IniDispatch::append_to`, this buffer is likely to be shared with other dispatches: again, you will not destroy the world nor generate errors, but you will make the next `IniDispatch::append_to`s useless. Therefore, **the property `IniDispatch::append_to` should be considered read-only** -- this is just a logical imposition (and this is why `IniDispatch::append_to` is not passed as `const`).

Typical peaceful edits are the calls of the functions `ini_collapse_array()` and `ini_unquote()` directly on the buffer `IniDispatch::value` (but be sure that you are not going to edit the global string `#INI_IMPLICIT_VALUE`):

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

  if (load_ini_file("my_file.ini", INI_DEFAULT_FORMAT, NULL, ini_listener, NULL)) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  return 0;

}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### SIZE OF THE DISPATCHED DATA

Within an INI file it is granted that if one sums together all the properties `IniDispatch::d_len + 1` and all the _non-zero_ properties `IniDispatch::v_len + 1`, the result will always be less-than or equal-to `IniStatistics::bytes` (where `+ 1` represents the NUL terminators). **If one adds to this also all the `IniDispatch::at_len` properties, or if the `IniDispatch::v_len` properties of implicit keys are non-zero, the sum may exceed it.** This can be relevant or irrelevant depending on your code.

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

  if (load_ini_file("my_file.ini", INI_DEFAULT_FORMAT, ini_init, ini_listener, &check)) {

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

For a correct use of this library it is fundamental to understand the `IniFormat` bitfield. **libconfini** has been born as a general INI parser, with the main purpose of _being able to understand INI files written by other programs_ (see README). Therefore some flexibility was required.

When an INI file is parsed it is parsed according to a format. The `IniFormat` bitfield is a description of such format.

Each format can be represented also as a univocal 24-bit unsigned integer. For converting an `IniFormat` to a number and vice versa please see `ini_format_get_id()`, `ini_format_set_to_id()` and `::IniFormatId`.

## THE `IniStatistics` AND `IniDispatch` STRUCTURES

When the `load_ini_file()` function reads an INI file, it dispatches the file content to the `f_foreach()` listener. Before the dispatching begins some statistics about the parsed file can be dispatched to the `f_init()` listener (if this is non-`NULL`).

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

Together with the functions listed above the following wrappers are available, in case you don't want to `#include <stdlib.h>` in your source:

* `ini_get_int()` = `atoi()`
* `ini_get_lint()` = `atol()`
* `ini_get_llint()` = `atoll()`
* `ini_get_float()` = `atof()`

## FORMATTING THE KEY NAMES

The function `ini_unquote()` may be useful for key names enclosed within quotes.

## FORMATTING THE SECTION PATHS

For retrieving the parts of a section path the functions `ini_array_get_length()`, `ini_array_foreach()` and `ini_split_array()` can be used with '.' as delimiter. Note that section paths dispatched by **libconfini** are _always_ collapsed arrays, therefore calling the function `ini_collapse_array()` on them will have no effects.

It may be required that the function `ini_unquote()` be applied to each part of a section path, depending on the content and the format of the INI file.
