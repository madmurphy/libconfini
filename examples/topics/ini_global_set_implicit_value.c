/*  examples/topics/ini_global_set_implicit_value.c  */

#include <stdio.h>
#include <confini.h>

#define NO 0
#define YES 1

static int ini_listener (IniDispatch * disp, void * v_null) {

	if (disp->value == INI_GLOBAL_IMPLICIT_VALUE) {

		printf(
			"\nDATA: %s\nVALUE: %s\nNODE TYPE: %d\n(This is an implicit key element)\n",
			disp->data, disp->value, disp->type
		);

	} else {

		printf("\nDATA: %s\nVALUE: %s\nNODE TYPE: %d\n", disp->data, disp->value, disp->type);

	}

	return 0;

}

int main () {

	IniFormat my_format = INI_UNIXLIKE_FORMAT;

	ini_global_set_implicit_value("[implicit default value]", 0);

	/*  Without setting this implicit keys will be considered empty  */
	my_format.implicit_is_not_empty = YES;

	if (load_ini_path("ini_files/unix-like.conf", my_format, NULL, ini_listener, NULL)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

}

