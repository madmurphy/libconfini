/* examples/topics/load_ini_path.c */

#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *v_null) {

	printf(
		"DATA: %s\nVALUE: %s\nNODE TYPE: %d\n\n",
		dispatch->data, dispatch->value, dispatch->type
	);

	return 0;

}

int main () {

	if (load_ini_path("example.conf", INI_DEFAULT_FORMAT, NULL, ini_listener, NULL)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	return 0;

}
