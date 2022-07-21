Get involved {#develop}
=======================

If you want to participate in the development of **libconfini**, the `dev`
subdirectory is where all the source files that matter are located (of both the
library and the documentation).

There is always room for improving the documentation, especially concerning
style, grammar and clarity.

If you want to propose fresh code, please make sure to apply your changes to
`dev/marked-sources/confini-marked.c` and `dev/marked-sources/confini-marked.h`
instead of `src/confini.c` and `src/confini.h` (the latter are automatically
generated). You can afterwards launch

``` sh
./configure --enable-author-mode
make all-official-sources
```

to update the `src` subdirectory.

Currently only two I/O standards are supported: the C Standard and the POSIX
Standard. If you believe that **libconfini** should also support an I/O API
provided by a non-standard header (such as `fsio.h`, `mem-ffs.h`, `efs.h` or
`ff.h`), please do not hesitate to [propose it][1].


Strangeness warning
-------------------

If you have had a look at the source code of **libconfini** already, you have
probably noticed something odd about it: the abundance of what seems to be
magic numbers.

The reason behind them has to do with the fact that the library needs to be
able to support thousands of INI dialects without slowing down. If you want to
understand how this works, please follow me.

The `IniFormat` bitfield occupies 24 bits. After collapsing the seven bits of
`IniFormat::delimiter_symbol` into one single bit (complexity-wise there are
only two meaningful states for this field: `INI_ANY_SPACE` and any normal
character) it becomes 18 bits wide. This corresponds to 2¹⁸ (262144) possible
INI formats. Three fields however make sense only if some other fields are set
to a particular state (`IniFormat::disabled_can_be_implicit` and
`IniFormat::disabled_after_space` are no-op when the format does not support
disabled entries, and `IniFormat::preserve_empty_quotes` is no-op when the
format does not support quotes); this lowers the number of available possible
formats down to 110592. It goes without saying that **libconfini** must support
all of them.

Many of the consequences that each format carries manifest during loops that
scroll through each character of the parsed INI file. Inserting conditions
inside loops creates slow code, and if these conditions are not going to change
during the loop, unacceptable code -- for instance, it does not make much sense
to check for every character whether a format supports quotes or not, this is
something that will remain constant during the entire parsing. The normal
solution would be that of keeping the constant conditions outside the loops and
creating slightly different loops for each case, but this is unthinkable with
110592 possibilities. There is however also a third way: bit masks. Bit masks
allow to collapse several conditions into one single operation and keep the
complexity constant.

Let us make a concrete example.

A format that supports quotes needs to check if a backslash precedes the quotes
or not. This check would be needed even if our parser supported only one single
format, it is simply part of the quotes feature.

Let us imagine that we create a boolean named `b_backslash_precedes` for that.
For every character the loop will have to check something similar to `chr ==
'"' && !b_backslash_precedes`.

With multiple formats though, the check would have to become `chr == '"' &&
!b_backslash_precedes && !b_format_no_quotes` -- with one more check to do, and
the ugly reality that `b_format_no_quotes` would remain constant for the entire
loop.

However, if `b_backslash_precedes` were the flag `1` of a bit mask and we used
`chr == '"' && !(bitmask & 1)` for the first check, adding also the second
check would simply transform the condition into `chr == '"' && !(bitmask & 3)`
-- with `b_format_no_quotes` corresponding to the bit `2` of the bit mask. No
operations would be added, only the numbers would change. The fact that we are
checking a constant on every character has become irrelevant; doing `bitmask &
3` will not be slower than doing `bitmask & 1`.

This example illustrates how bit masks can collapse several conditions into
single operations and ease the repercussions on the final performance.
Unfortunately the conditions **libconfini** has to check are usually never just
two, but dozens each time. And while bit masks do keep the number of branches
constant, they definitely look esoteric.

The usual way to avoid esoteric code is that of using `enum` labels for
bitflags. With **libconfini** however this would not help much: bitflags are
used so often that it would just fill the entire source code of `enum`s. The
esoterism would just change form.

I am sorry if the code scares you, but there are no other ways, I am afraid.¹
If you want to understand how the library works you will have to get used to
deal with magic numbers. Once you have learned how to read them, though, they
will look a bit less magic; or at least not more magic than any other ordinary
ollection of bit flags.


Notes
-----

1. Lately it has been explored the possibility of using the m4 preprocessor
   for templating the functions that make heavy use of bit masks. This solution
   seems to create an acceptable compromise; however it has been explored only
   very recently, and rewriting in another language code that already works
   only for the sake of having human-friendly labels is not a worthy goal.
   The finding will make it possible nonetheless, from now on, to create such
   m4 templates “on demand” every time a bug in one of the existing functions
   is discovered (for facilitating the analysis), or when a new feature is
   introduced.


  [1]: https://github.com/madmurphy/libconfini/issues

