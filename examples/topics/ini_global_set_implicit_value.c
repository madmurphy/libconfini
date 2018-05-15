/*  examples/topics/ini_global_set_implicit_value.c  */

#include <stdio.h>
#include <confini.h>

#define NO 0
#define YES 1

int ini_listener (IniDispatch * dispatch, void * v_null) {

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

	IniFormat my_format = INI_DEFAULT_FORMAT;

	ini_global_set_implicit_value("[implicit default value]", 0);

	/*  Without setting this, implicit keys will be considered empty:  */
	my_format.implicit_is_not_empty = YES;

	if (load_ini_path("ini_files/typed_ini.conf", my_format, NULL, ini_listener, NULL)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	return 0;

}

