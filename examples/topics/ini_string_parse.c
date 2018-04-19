/* examples/topics/ini_string_parse.c */

#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *v_null) {

	if (dispatch->type == INI_KEY || dispatch->type == INI_DISABLED_KEY) {

		ini_unquote(dispatch->data, dispatch->format);
		ini_string_parse(dispatch->value, dispatch->format);

	}

	printf("DATA: %s\nVALUE: %s\n", dispatch->data, dispatch->value);

	return 0;

}

int main () {

	if (load_ini_path("example.conf", INI_DEFAULT_FORMAT, NULL, ini_listener, NULL)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	return 0;

}
