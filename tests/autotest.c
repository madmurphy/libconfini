/*\
|*|	tests/autotest.c
|*|
|*|	This program is invoked by **GNU Make** with different flags during
|*|	separate build/installation stages. Use `make check`, `make
|*|	installcheck` and `make finishcheck` to run this program.
\*/


/*  Pass `CPPFLAGS='-DTESTS_PRINTF_HEADER="<some_header.h>"'` to use a non-standard header  */
#ifndef TESTS_PRINTF_HEADER
#define TESTS_PRINTF_HEADER <stdio.h>
#endif

/*
	Pass `CPPFLAGS='-DSTDOUT_FUNCTION=some_printf_function'` to use a
	different function name instead of `printf()`
*/
#ifndef TESTS_PRINTF_FUNCTION
#define TESTS_PRINTF_FUNCTION printf
#endif

#include TESTS_PRINTF_HEADER
#ifndef TESTS_CONFINI_HEADER
/*  possibly overridden via `-DTESTS_CONFINI_HEADER=[HEADER-NAME]`  */
#define TESTS_CONFINI_HEADER "confini.h"
#endif
#include TESTS_CONFINI_HEADER

char ini_cache[] = "testkey=testvalue";

static int test_callback (IniDispatch * dispatch, void * v_null) {

	TESTS_PRINTF_FUNCTION(
		"DATA: %s, VALUE: %s, NODE TYPE: %u\n",
		dispatch->data, dispatch->value, dispatch->type
	);

	return 0;

}

int main () {

	const int retval = strip_ini_cache(
		ini_cache,
		sizeof(ini_cache) - 1,
		INI_DEFAULT_FORMAT,
		NULL,
		test_callback,
		NULL
	);

	if (retval) {

		TESTS_PRINTF_FUNCTION("error: strip_ini_cache() exited with status %d\n", retval);
		return 1;

	}

	return 0;

}

