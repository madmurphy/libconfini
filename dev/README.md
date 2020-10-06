Get involved {#develop}
=======================

If you want to participate in the development of **libconfini**, the `dev`
subdirectory is where all the source files that matter are located (of both the
library and the documentation).

There is always room for improving the documentation, especially concerning
style, grammar and clarity.

If you want to propose fresh code, please make sure to apply your changes to
`dev/marked-sources/confini-marked.c` and `dev/marked-sources/confini-marked.h`
instead of `src/confini.c` and `src/confini.h` (as the latter are automatically
generated). You can afterwards launch

```````````````````````````````` sh
./configure --enable-author-mode
make all-official-sources
````````````````````````````````

to update the `src` subdirectory.

Currently only two I/O standards are supported: the C Standard and the POSIX
Standard. If you think that **libconfini** should also support an I/O API
provided by a non-standard header (such as `fsio.h`, `mem-ffs.h`, `efs.h` or
`ff.h`), please do not hesitate to [propose it][1].


  [1]: https://github.com/madmurphy/libconfini/issues

