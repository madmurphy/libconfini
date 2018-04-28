/* examples/miscellanea/ambiguous.c */

#include <stdio.h>
#include <confini.h>

int ini_listener (IniDispatch *dispatch, void *v_null) {

	printf(
		"NODE #%d - TYPE: %d, DATA: \"%s\", VALUE: \"%s\"\n",
		dispatch->dispatch_id, dispatch->type, dispatch->data, dispatch->value
	);

	return 0;

}

int main () {

	#define NO 0
	#define YES 1

	IniFormat my_format = {
		.delimiter_symbol = INI_EQUALS,
		.semicolon_marker = INI_DISABLED_OR_COMMENT,
		.hash_marker = INI_ONLY_COMMENT,
		.multiline_nodes = INI_MULTILINE_EVERYWHERE,
		.case_sensitive = NO,
		.no_spaces_in_names = NO,
		.no_single_quotes = NO,
		.no_double_quotes = NO,
		.implicit_is_not_empty = YES,
		.do_not_collapse_values = NO,
		.preserve_empty_quotes = NO,
		.no_disabled_after_space = YES,
		.disabled_can_be_implicit = YES
	};

	#undef NO
	#undef YES

	printf(":: Content of \"ambiguous.conf\" ::\n\n");

	if (load_ini_path("ini_files/ambiguous.conf", my_format, NULL, ini_listener, NULL)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	return 0;

}

