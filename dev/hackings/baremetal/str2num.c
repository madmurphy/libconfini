#define _LIBCONFINI_RADIX_POINT_ '.'
#define __INTEGER_DATA_TYPE__ 0
#define __FLOAT_DATA_TYPE__ 1


/*

Mask `abcd` (6 bits used):

	FLAG_1		Continue the loop
	FLAG_2		The numerical part has been found
	FLAG_4		This is the sign
	FLAG_8		Radix point has not been encountered yet
	FLAG_16		This is a digit
	FLAG_32		The number is negative

*/
#define _LIBCONFINI_STR2NUM_FNBODY_(HAS_RADIX_PT, DATA_TYPE, STR) \
	register uint_least8_t abcd = 9; \
	register size_t idx = 0; \
	register DATA_TYPE retval = 0; \
	__PP_IIF__(HAS_RADIX_PT, \
		DATA_TYPE fact = 1; \
	) \
	do { \
		abcd	=	STR[idx] > 47 && STR[idx] < 58 ? \
						abcd | 18 \
					: !(abcd & 6) && STR[idx] == '-' ? \
						(abcd & 47) | 36 \
					: !(abcd & 6) && STR[idx] == '+' ? \
						(abcd & 15) | 4 \
					__PP_IIF__(HAS_RADIX_PT, \
					: (abcd & 8) && STR[idx] == _LIBCONFINI_RADIX_POINT_ ? \
						(abcd & 39) | 2 \
					) \
					: !(abcd & 2) && is_some_space(STR[idx], _LIBCONFINI_WITH_EOL_) ? \
						(abcd & 47) | 1 \
					: \
						abcd & 46; \
		if (abcd & 16) { \
			retval	=	__PP_IIF__(HAS_RADIX_PT, \
						abcd & 8 ? \
							STR[idx] - 48 + retval * 10 \
						: \
							(STR[idx] - 48) / (fact *= 10) + retval, \
						retval * 10 + STR[idx] - 48 \
						); \
		} \
		idx++; \
	} while (abcd & 1); \
	return abcd & 32 ? -retval : retval;


int ini_get_int (const char * const ini_string) {
	_LIBCONFINI_STR2NUM_FNBODY_(__INTEGER_DATA_TYPE__, int, ini_string);
}


long int ini_get_lint (const char * const ini_string) {
	_LIBCONFINI_STR2NUM_FNBODY_(__INTEGER_DATA_TYPE__, long int, ini_string);
}


long long int ini_get_llint (const char * const ini_string) {
	_LIBCONFINI_STR2NUM_FNBODY_(__INTEGER_DATA_TYPE__, long long int, ini_string);
}


float ini_get_float (const char * const ini_string) {
	_LIBCONFINI_STR2NUM_FNBODY_(__FLOAT_DATA_TYPE__, float, ini_string);
}


double ini_get_double (const char * const ini_string) {
	_LIBCONFINI_STR2NUM_FNBODY_(__FLOAT_DATA_TYPE__, double, ini_string);
}
