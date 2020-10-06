Build Environment Change Log
============================


## 1.15.0

Changes:

* Options `--enable-dllexp`, `--disable-man` and `--without-libc` have been
  added to the `configure` script; the last option is a split from the former
  † `--with-io-api=nolibc` case
* New **GNU  Make** target `make all-baremetal-sources` has been created;
  former targets † `make baremetal-csources`, † `make expired`, † `make
  official-csources`, † `make official-sources` have been renamed to `make
  baremetal-source-code`, `make refresh`, `make official-source-code`, `make
  all-official-sources`


## 1.13.0

Changes:

* A `--without-io-api` option has been added to the `configure` script for
  compiling the library without the C Standard I/O API
* **GNU Autotests** have been implemented (via `make check`, `make finishcheck`
  and `make installcheck` -- launch `make help` for more information about
  these targets)
* Code review in the `configure` script (options `--disable-maintainer-mode`,
  `--enable-author-mode` and `--enable-htmldoc` have been created; the `xargs`
  utility has been removed from the requirements for building the package) and
  in the **GNU Make** environment (`Makefile.am`: the following targets have
  been created: `make all-sources`, † `make baremetal-csources`,
  † `make expired`, † `make official-csources`, † `make official-sources`,
  `make official-symbols` and `make snapshot`, furthermore
  † `make author-clean` has been renamed to `make bootstrap-clean`)


## 1.11.0

Changes:

* Code review in the `configure` script (created a `--with-io-api=API` option
  for manually selecting the I/O API to use) and in the **GNU Make**
  environment (general review; created a `make git-clone` target)


## 1.10.4

Changes:

* Code review in the `configure` script (made sure not to overwrite the
  user-given `--docdir=[DIR]` argument; created a `--disable-devel` option for
  generating binary-only installations -- i.e. without installing headers,
  static libraries, documentation, examples, etc.) and in the **GNU Make**
  environment (`Makefile.am`: removed † `make binary-image`, † `make
  source-image` and † `make authors-suitcase` targets, renamed † `make
  install-manifest` target to `make manifest`, created `make portable-builds`
  and `make oblivion-clean` targets, improved `install-data-hook` and all the
  helper targets; `src/Makefile.am`: moved the `CFLAGS` automatically guessed by
  `AC_PROG_CC_C99` to `AM_CFLAGS`)


## 1.10.3

Changes:

* The † `autogen.sh` script has been renamed to `bootstrap`
* Code review in the **Autotools** environment (fixed `make distcheck` fail;
  made sure that the `configure` and `bootstrap` scripts can be launched from
  any path; created † `make source-image`, † `make authors-suitcase`, `make
  source-release` and `make authors-copy` targets for better reproducing the
  content of the package as released by its authors; created † `make
  binary-image`, `make binary-release` and † `make install-manifest` targets
  for aiding staged installations; created a † `make author-clean` target for
  cleaning the source directory with the same degree of fury of `./bootstrap
  --clean`; created a `make help` target for printing the list of commonly used
  targets; replaced † `autogen.sh --multiversion` with `./configure
  --with-other-versions` for creating a package able to coexist with other
  versions of itself)


## 1.10.2

Changes:

* The `configure` script has been improved with a built-in support for complete
  package renaming (using `--program-prefix=PREFIX`, `--program-suffix=SUFFX`
  and `--program-transform-name=s/[OLDTEXT]/[NEWTEXT]/[g]`) and several new
  options (`--enable-version-info` and `--disable-version-info`, for
  enabling/disabling **libtool** versioning system; `--enable-extended-config`,
  for safely exporting package renamings at `configure` time, or any changes in
  `configure.ac`, directly to `src/winres.rc` and `package.json` -- see
  `INSTALL` and `./configure --help` for more information)
* The package tree has been re-organized -- all the files needed only at build
  time have been moved into the `autostuff` subdirectory


## 1.9.0

Changes:

* Added † `--multiversion` option to the † `autogen.sh` script for future major
  version changes (see `INSTALL`)
* Added `--with-pkgconfigdir=DIR` option to the `configure` script


## 1.8.4

Changes:

* † `autogen.sh`: replaced `#!/bin/sh` with `#!/bin/bash` -- see issue
  [#7](https://github.com/madmurphy/libconfini/issues/7)


## 1.8.2

Changes:

* Optimization flag `-O2` seems to be working better than `-O3` on Unix
  systems: conditionally use `-O2` or `-O3` depending on the host
* Use **libtool**'s `-avoid-version` only when compiling under native Microsoft
  Windows environment
* Added `--clean` option to the † `autogen.sh` script
* Updated **Autotools** macros


## 1.8.1

Changes:

* Updated **Autotools** macros


## 1.8.0

Changes:

* Introduced **libtool** versioning system for the compiled binary (versioning
  begins with current version 1.8.0, represented as `0:0:0`); note that
  **libtool** versioning system does not affect the versioning of the project
  currently in use, but constitutes only a means for the linker to keep track
  of incompatibilities between different releases of the library
* Improved support for building **libconfini** under Microsoft Windows (created
  Windows resource file `src/winres.rc`; conditionally added option
  `-avoid-version` to `LDFLAGS` in order to skip foreign **libtool** versioning
  system under Microsoft Windows; created batch script `mgwmake.bat` for
  compiling **libconfini** under Microsoft Windows without **Autotools**)
* Updated **Autotools** macros


## 1.7.1

Changes:

* Added `--disable-doc` and `--disable-examples` options to the `configure`
  script
* Added `--noconfigure` and `--help` options to the † `autogen.sh` script


## 1.5.0 (unstable)

Changes:

* Removed unused internationalization support from **Autotools**


## 1.4.0 (unstable)

Changes:

* Version numbering standardized (`${major}.${minor}.${revision}` instead of
  `${major}.${minor}-${release}`)

