/*  examples/miscellanea/typed_ini.c  */
/*

The following code will try to read an INI section called `my_section`,
expected to contain the following typed data:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.ini}

[my_section]

my_string = [string]
my_number = [number]
my_boolean = [boolean]
my_implicit_boolean
my_array = [comma-delimited array]

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No errors will be generated if any of the data above are absent.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <confini.h>

#define MY_ARRAY_DELIMITER ','

/*  My stored data  */
struct ini_store {
	char * my_section_my_string;
	signed int my_section_my_number;
	bool my_section_my_boolean;
	bool my_section_my_implicit_boolean;
	char ** my_section_my_array;
	size_t my_section_my_arr_len;
};

static int my_init (IniStatistics * statistics, void * v_store) {

	*((struct ini_store *) v_store) = (struct ini_store) { NULL, -1, false, false, NULL, 0 };

	return 0;

}
 
static char ** make_strarray (
	size_t * arrlen,
	const char * src,
	const size_t buffsize,
	IniFormat ini_format
) {
 
	*arrlen = ini_array_get_length(src, MY_ARRAY_DELIMITER, ini_format);

	char ** const dest = *arrlen ? (char **) malloc(*arrlen * sizeof(char *) + buffsize) : NULL;

	if (!dest) { return NULL; }

	memcpy(dest + *arrlen, src, buffsize);

	char * iter = (char *) (dest + *arrlen);

	for (size_t idx = 0; idx < *arrlen; idx++) {

		dest[idx] = ini_array_release(&iter, MY_ARRAY_DELIMITER, ini_format);
		ini_string_parse(dest[idx], ini_format);

	}

	return dest;

}

static int my_handler (IniDispatch * this, void * v_store) {

	struct ini_store * store = (struct ini_store *) v_store;

	if (this->type == INI_KEY && ini_string_match_si("my_section", this->append_to, this->format)) {

		if (ini_string_match_si("my_string", this->data, this->format)) {

			this->v_len = ini_string_parse(this->value, this->format);

			/*  Allocate a new string  */
			if (store->my_section_my_string) { free(store->my_section_my_string); }
			store->my_section_my_string = strndup(this->value, this->v_len);
			if (!store->my_section_my_string) { return 1; }

		} else if (ini_string_match_si("my_number", this->data, this->format)) {

			store->my_section_my_number = ini_get_int(this->value);

		} else if (ini_string_match_si("my_boolean", this->data, this->format)) {

			store->my_section_my_boolean = ini_get_bool(this->value, 1);

		} else if (ini_string_match_si("my_implicit_boolean", this->data, this->format)) {

			store->my_section_my_implicit_boolean = ini_get_bool(this->value, 1);

		} else if (ini_string_match_si("my_array", this->data, this->format)) {

			/*  Save memory (not strictly needed)  */
			this->v_len = ini_array_collapse(this->value, MY_ARRAY_DELIMITER, this->format);

			/*  Allocate a new array of strings  */
			if (store->my_section_my_array) { free(store->my_section_my_array); }
			store->my_section_my_array = make_strarray(&store->my_section_my_arr_len, this->value, this->v_len + 1, this->format);
			if (!store->my_section_my_array) { return 1; }

		}

	}

	return 0;

}

static void print_stored_data (const struct ini_store * const store) {

	printf(

		"my_string -> %s\n"
		"my_number -> %d\n"
		"my_boolean -> %s\n"
		"my_implicit_boolean -> %s\n"
		"my_array[%d] -> [%s",

		store->my_section_my_string,
		store->my_section_my_number,
		store->my_section_my_boolean ? "True (`1`)" : "False (`0`)",
		store->my_section_my_implicit_boolean ? "True (`1`)" : "False (`0`)",
		store->my_section_my_arr_len,
		store->my_section_my_arr_len ? store->my_section_my_array[0] : ""

	);

	for (size_t idx = 1; idx < store->my_section_my_arr_len; idx++) {

		printf("|%s", store->my_section_my_array[idx]);

	}

	printf("]\n");

}

int main () {

	IniFormat my_format;

	struct ini_store my_store;

	ini_global_set_implicit_value("YES", 0);
	my_format = INI_DEFAULT_FORMAT;
	my_format.semicolon_marker = my_format.hash_marker = INI_IGNORE;
	my_format.implicit_is_not_empty = true;

	if (load_ini_path("ini_files/typed_ini.conf", my_format, my_init, my_handler, &my_store)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	print_stored_data(&my_store);

	if (my_store.my_section_my_string) {

		free(my_store.my_section_my_string);

	}

	if (my_store.my_section_my_arr_len) {

		free(my_store.my_section_my_array);

	}

	return 0;

}

