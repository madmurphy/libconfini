/* examples/miscellanea/print_format.c */

#include <stdio.h>
#include <confini.h>

#define NO 0
#define YES 1

void ini_print_format (IniFormat format) {

	#define __AS_STRING__(SIZE, PROPERTY, DEFVAL) "    ." #PROPERTY " = %d,\n"
	#define __AS_ARG__(SIZE, PROPERTY, DEFVAL) , format.PROPERTY
	#define __C_SOURCE__ "format = {\n" _LIBCONFINI_INIFORMAT_AS_(__AS_STRING__) "};\n" _LIBCONFINI_INIFORMAT_AS_(__AS_ARG__)

	printf(__C_SOURCE__);

	#undef __C_SOURCE__
	#undef __AS_ARG__
	#undef __AS_STRING__

}

int main () {

	IniFormat my_format = {
		.delimiter_symbol = ':',
		.case_sensitive = NO,
		.semicolon_marker = INI_DISABLED_OR_COMMENT,
		.hash_marker = INI_DISABLED_OR_COMMENT,
		.section_paths = INI_ABSOLUTE_AND_RELATIVE,
		.multiline_nodes = INI_MULTILINE_EVERYWHERE,
		.no_single_quotes = NO,
		.no_double_quotes = NO,
		.no_spaces_in_names = YES,
		.implicit_is_not_empty = YES,
		.do_not_collapse_values = NO,
		.preserve_empty_quotes = NO,
		.no_disabled_after_space = NO,
		.disabled_can_be_implicit = NO
	};

	ini_print_format(my_format);

	return 0;

}
