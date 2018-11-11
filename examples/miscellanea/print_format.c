/*  examples/miscellanea/print_format.c  */
/*

The following code will try to convert an `IniFormatNum` to an `IniFormat` and
print its content. Then the generated `IniFormat` will be converted back to its
`IniFormatNum`.

*/

#include <stdio.h>
#include <confini.h>

#define __IF_AS_PFSTRING__(PROPERTY, OFFSET, SIZE, DEFVAL) "        ." #PROPERTY " = %d,\n"
#define __IF_AS_PFARG__(PROPERTY, OFFSET, SIZE, DEFVAL) , FORMAT_TO_PRINT.PROPERTY
#define X_PRINT_INI_FORMAT_SOURCE() \
  printf("    my_format = {\n" INIFORMAT_TABLE_AS(__IF_AS_PFSTRING__) "    };\n" \
  INIFORMAT_TABLE_AS(__IF_AS_PFARG__))


int main () {

  #define MY_FORMAT_NUM 786490

  printf("Format No. %u:\n\n", (IniFormatNum) MY_FORMAT_NUM);

  IniFormat my_format = ini_ntof(MY_FORMAT_NUM);

  /* Needed for the X macro `X_PRINT_INI_FORMAT_SOURCE()` */
  #define FORMAT_TO_PRINT my_format

  X_PRINT_INI_FORMAT_SOURCE();

  #undef FORMAT_TO_PRINT

  printf("\n    ini_fton(my_format) ==> %u\n", ini_fton(my_format));

  #undef MY_FORMAT_NUM

  return 0;

}

#undef X_PRINT_INI_FORMAT_SOURCE
#undef __IF_AS_PFARG__
#undef __IF_AS_PFSTRING__

