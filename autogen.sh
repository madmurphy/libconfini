#!/bin/sh
# Run this to generate all the initial makefiles, etc.

DIE=0
CONFIGURE_ARGS=()

for AG_ARG; do
	case "${AG_ARG}" in
		-h|--help)
			echo "Usage: ${0} [option]"
			echo
			echo 'Options:'
			echo "        -h|--help               Show this help message"
			echo "        -n|--noconfigure        Skip the configure process"
			echo
			echo 'If this script is invoked without the `--noconfigure'\'' option all unrecognized'
			echo 'arguments will be passed to `configure'\''.'
			exit 0
			;;
		-n|--noconfigure)
			NOCONFIGURE='YES'
			;;
		*)
			CONFIGURE_ARGS+=("${AG_ARG}")
			;;
	esac
done

srcdir=`dirname ${0}`
test -z "${srcdir}" && srcdir=.

if [[ $([[ -z "${OSTYPE}" ]] && uname -s || echo "${OSTYPE}") =~ ^[Dd]arwin* ]]; then
	if [[ -z "${LIBTOOL}" ]]; then
		echo "macOS detected. Using glibtool."
		LIBTOOL='glibtool'
	fi
	if [[ -z "${LIBTOOLIZE}" ]]; then
		echo "macOS detected. Using glibtoolize."
		LIBTOOLIZE='glibtoolize'
	fi
else
	if [[ -z "${LIBTOOL}" ]]; then
		LIBTOOL='libtool'
	fi
	if [[ -z "${LIBTOOLIZE}" ]]; then
		LIBTOOLIZE='libtoolize'
	fi
fi

if [ -n "${GNOME2_DIR}" ]; then
	ACLOCAL_FLAGS="-I ${GNOME2_DIR}/share/aclocal ${ACLOCAL_FLAGS}"
	LD_LIBRARY_PATH="${GNOME2_DIR}/lib:${LD_LIBRARY_PATH}"
	PATH="${GNOME2_DIR}/bin:${PATH}"
	export PATH
	export LD_LIBRARY_PATH
fi

(test -f "${srcdir}/configure.ac") || {
	echo -n "**Error**: Directory \`${srcdir}' does not look like the"
	echo " top-level package directory"
	exit 1
}

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "**Error**: You must have \`autoconf' installed."
	echo "Download the appropriate package for your distribution,"
	echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	DIE=1
}

(grep "^IT_PROG_INTLTOOL" "${srcdir}/configure.ac" >/dev/null) && {
	(intltoolize --version) < /dev/null > /dev/null 2>&1 || {
		echo 
		echo "**Error**: You must have \`intltool' installed."
		echo "You can get it from:"
		echo "  ftp://ftp.gnome.org/pub/GNOME/"
		DIE=1
	}
}

(grep "^AM_PROG_XML_I18N_TOOLS" "${srcdir}/configure.ac" >/dev/null) && {
	(xml-i18n-toolize --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`xml-i18n-toolize' installed."
		echo "You can get it from:"
		echo "  ftp://ftp.gnome.org/pub/GNOME/"
		DIE=1
	}
}

(grep "^LT_INIT" "${srcdir}/configure.ac" >/dev/null) && {
	("${LIBTOOL}" --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`libtool' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		DIE=1
	}
}

(grep "^AM_GLIB_GNU_GETTEXT" "${srcdir}/configure.ac" >/dev/null) && {
	(grep "sed.*POTFILES" "${srcdir}/configure.ac") > /dev/null || \
	(glib-gettextize --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`glib' installed."
		echo "You can get it from: ftp://ftp.gtk.org/pub/gtk"
		DIE=1
	}
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "**Error**: You must have \`automake' installed."
	echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
	DIE=1
	NO_AUTOMAKE=yes
}


# if no automake, don't bother testing for aclocal
test -n "${NO_AUTOMAKE}" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "**Error**: Missing \`aclocal'.  The version of \`automake'"
	echo "installed doesn't appear recent enough."
	echo "You can get automake from ftp://ftp.gnu.org/pub/gnu/"
	DIE=1
}

if test "${DIE}" -eq 1; then
	exit 1
fi

if test "x${NOCONFIGURE}" = x; then
	echo "I am going to prepare the build system and then run the \`configure' script."
	if test -z "$*"; then
		echo
		echo "**Warning**: I am going to run \`configure' with no arguments."
		echo "If you wish to pass any to it, please specify them on the"
		echo "\`${0}' command line."
		echo
	fi
else
	echo "I am going to prepare the build system without running the \`configure' script."
fi

echo 'Preparing the build system... please wait'

case "${CC}" in
	xlc )
		am_opt=--include-deps
		;;
esac

for coin in `find "${srcdir}" -path "${srcdir}/CVS" -prune -o -name configure.ac -print`
do 
	dr=`dirname "${coin}"`
	if test -f "${dr}/NO-AUTO-GEN"; then
		echo "skipping ${dr} -- flagged as no auto-gen"
	else
		echo "Processing '${dr}' ..."
		( cd "${dr}"

			aclocalinclude="${ACLOCAL_FLAGS}"

			if grep "^AM_GLIB_GNU_GETTEXT" configure.ac >/dev/null; then
				echo "Creating ${dr}/aclocal.m4 ..."
				test -r "${dr}/aclocal.m4" || touch "${dr}/aclocal.m4"
				echo "Running glib-gettextize...  Ignore non-fatal messages."
				echo "no" | glib-gettextize --force --copy
				echo "Making ${dr}/aclocal.m4 writable ..."
				test -r "${dr}/aclocal.m4" && chmod u+w "${dr}/aclocal.m4"
			fi
			if grep "^IT_PROG_INTLTOOL" configure.ac >/dev/null; then
				echo "Running intltoolize..."
				intltoolize --copy --force --automake
			fi
			if grep "^AM_PROG_XML_I18N_TOOLS" configure.ac >/dev/null; then
				echo "Running xml-i18n-toolize..."
				xml-i18n-toolize --copy --force --automake
			fi
			if grep "^LT_INIT" configure.ac >/dev/null; then
				if test -z "${NO_LIBTOOLIZE}" ; then 
					echo "Running libtoolize..."
					"${LIBTOOLIZE}" --force --copy
				fi
			fi
			echo "Running aclocal ${aclocalinclude} ..."
			aclocal ${aclocalinclude}
			if grep "^A[CM]_CONFIG_HEADER" configure.ac >/dev/null; then
				echo "Running autoheader..."
				autoheader
			fi
			echo "Running automake --gnu ${am_opt} ..."
			automake --add-missing --copy --gnu ${am_opt}
			echo "Running autoconf ..."
			autoconf
		)
	fi
done

if test "x${NOCONFIGURE}" = x; then
	echo -n "Running ${srcdir}/configure"
	for CF_ARG in "${CONFIGURE_ARGS[@]}"; do
		echo -n " [${CF_ARG}]"
	done
	echo " ..."
	"${srcdir}/configure" "${CONFIGURE_ARGS[@]}" \
	&& echo 'Now type `make'\'' to compile.' || exit 1
else
	echo 'Skipping configure process. Type `configure --help'\'' to list the configure'
	echo 'options.'
fi

