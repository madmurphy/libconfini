/*  examples/topics/ini_string_parse.c  */

#include <stdio.h>
#include <confini.h>

static int ini_listener (IniDispatch * disp, void * v_null) {

	if (disp->type == INI_KEY || disp->type == INI_DISABLED_KEY) {

		ini_unquote(disp->data, disp->format);
		ini_string_parse(disp->value, disp->format);

	}

	printf("DATA: %s\nVALUE: %s\n\n", disp->data, disp->value);

	return 0;

}

int main () {

	if (load_ini_path("ini_files/example.conf", INI_DEFAULT_FORMAT, NULL, ini_listener, NULL)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	return 0;

}

