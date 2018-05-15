/*  examples/miscellanea/print_format.c  */

#include <stdio.h>
#include <confini.h>

#define NO 0
#define YES 1

static void print_my_format (IniFormat format) {

	#define __AS_STRING__(SIZE, PROPERTY, DEFVAL) "        ." #PROPERTY " = %d,\n"
	#define __AS_ARG__(SIZE, PROPERTY, DEFVAL) , format.PROPERTY
	#define __C_SOURCE__ "    format = {\n" _LIBCONFINI_INIFORMAT_AS_(__AS_STRING__) "    };\n" _LIBCONFINI_INIFORMAT_AS_(__AS_ARG__)

	printf(__C_SOURCE__);

	#undef __C_SOURCE__
	#undef __AS_ARG__
	#undef __AS_STRING__

}

int main () {

	#define MY_FORMAT_NUM 786490

	printf("Format No. %d:\n\n", MY_FORMAT_NUM);

	print_my_format(ini_ntof(MY_FORMAT_NUM));

	return 0;

}

