/* examples/miscellanea/typed_ini.c */
/*

The following code will try to read an INI section called `my_section`, expected to
contain the following typed data:

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
#include <confini.h>

#define FALSE 0
#define TRUE 1
#define bool unsigned char

#define MY_ARRAY_DELIMITER ','

/* My stored data */
struct ini_store {
	char *my_section_my_string;
	signed int my_section_my_number;
	bool my_section_my_boolean;
	bool my_section_my_implicit_boolean;
	char **my_section_my_array;
	size_t my_section_my_arr_len;
	int _read_status_;
};

static int array_part_handler (char *part, size_t part_len, size_t idx, IniFormat format, void *v_array) {

	ini_string_parse(part, format);
	((char **) v_array)[idx] = part;
	return 0;

}
 
static char ** make_array (size_t * arrlen, const char * src, const size_t buffsize, IniFormat ini_format) {
 
	char ** dest;
	*arrlen = ini_array_get_length(src, MY_ARRAY_DELIMITER, ini_format);
	dest = (char **) malloc(*arrlen * sizeof(char *) + buffsize * sizeof(char));
	char * const str_ptr = (char *) dest + *arrlen * sizeof(char *);
	memcpy(str_ptr, src, buffsize);
	ini_array_split(str_ptr, MY_ARRAY_DELIMITER, ini_format, array_part_handler, dest);
 
	return dest;
 
}

static int my_ini_listener (IniDispatch *this, void *v_store) {

	struct ini_store *store = (struct ini_store *) v_store;

	switch (this->type) {

		case INI_SECTION:

			store->_read_status_ = ini_string_match_si("my_section", this->data, this->format) ? 1 : store->_read_status_ | 2;
			break;

		case INI_KEY:

			if (store->_read_status_ == 1) {

				if (ini_string_match_si("my_string", this->data, this->format)) {

					this->v_len = ini_string_parse(this->value, this->format);

					/* Allocate a new string */
					if (store->my_section_my_string) { free(store->my_section_my_string); }
					store->my_section_my_string = strndup(this->value, this->v_len);

				} else if (ini_string_match_si("my_number", this->data, this->format)) {

					store->my_section_my_number = ini_get_int(this->value);

				} else if (ini_string_match_si("my_boolean", this->data, this->format)) {

					store->my_section_my_boolean = ini_get_bool(this->value, 1);

				} else if (ini_string_match_si("my_implicit_boolean", this->data, this->format)) {

					store->my_section_my_implicit_boolean = ini_get_bool(this->value, 1);

				} else if (ini_string_match_si("my_array", this->data, this->format)) {

					this->v_len = ini_array_collapse(this->value, MY_ARRAY_DELIMITER, this->format); /* Save memory (not strictly needed) */

					/* Allocate a new array of strings */
					if (store->my_section_my_array) { free(store->my_section_my_array); }
					store->my_section_my_array = make_array(&store->my_section_my_arr_len, this->value, this->v_len + 1, this->format);

				}

			}

			break;

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

	struct ini_store my_store = { NULL, -1, FALSE, FALSE, NULL, 0, 0 };

	ini_global_set_implicit_value("YES", 0);
	my_format = INI_DEFAULT_FORMAT;
	my_format.semicolon_marker = my_format.hash_marker = INI_IGNORE;
	my_format.implicit_is_not_empty = TRUE;

	if (load_ini_path("typed_ini.conf", my_format, NULL, my_ini_listener, &my_store)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	my_store._read_status_ &= 1;
	print_stored_data(&my_store);

	if (!my_store._read_status_) {

		printf("\nSection `[my_section]` has not been found.\n");

	}

	if (my_store.my_section_my_string) {

		free(my_store.my_section_my_string);

	}

	if (my_store.my_section_my_arr_len) {

		free(my_store.my_section_my_array);

	}

	return 0;

}

