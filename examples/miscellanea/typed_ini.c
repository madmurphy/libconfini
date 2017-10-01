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
	unsigned int my_section_my_arr_len;
	int _read_status_;
};

static int populate_array (char *part, size_t part_len, size_t idx, IniFormat format, void *v_array) {

	ini_unquote(part, format);
	((char **) v_array)[idx] = part;
	return 0;

}

static int my_ini_listener (IniDispatch *this, void *v_store) {

	struct ini_store *store = (struct ini_store *) v_store;

	switch (this->type) {

		case INI_SECTION:

			store->_read_status_ = ini_string_match_si("my_section", this->data, this->format) ? 1 : store->_read_status_ | 2;
			break;

		case INI_KEY:

			if (store->_read_status_ == 1) {

				this->d_len = ini_unquote(this->data, this->format);

				if (ini_string_match_ss("my_string", this->data, this->format)) {

					this->v_len = ini_unquote(this->value, this->format);
					store->my_section_my_string = (char *) malloc((this->v_len + 1) * sizeof(char));
					memcpy(store->my_section_my_string, this->value, this->v_len + 1);

				} else if (ini_string_match_ss("my_number", this->data, this->format)) {

					store->my_section_my_number = ini_get_int(this->value);

				} else if (ini_string_match_ss("my_boolean", this->data, this->format)) {

					store->my_section_my_boolean = ini_get_bool(this->value, 1);

				} else if (ini_string_match_ss("my_implicit_boolean", this->data, this->format)) {

					store->my_section_my_implicit_boolean = ini_get_bool(this->value, 1);

				} else if (ini_string_match_ss("my_array", this->data, this->format)) {

					/* Save memory (not strictly needed) */
					this->v_len = ini_collapse_array(this->value, MY_ARRAY_DELIMITER, this->format);

					/* Allocate a new array of strings */
					store->my_section_my_arr_len = ini_array_get_length(this->value, MY_ARRAY_DELIMITER, this->format);
					store->my_section_my_array = (char **) malloc(store->my_section_my_arr_len * sizeof(char *) + (this->v_len + 1) * sizeof(char));
					char * const str_ptr = (char *) store->my_section_my_array + store->my_section_my_arr_len * sizeof(char *);
					memcpy(str_ptr, this->value, this->v_len + 1);

					/* Populate the array */
					ini_split_array(str_ptr, MY_ARRAY_DELIMITER, this->format, populate_array, store->my_section_my_array);

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
		store->my_section_my_boolean ? "true (`1`)" : "false (`0`)",
		store->my_section_my_implicit_boolean ? "true (`1`)" : "false (`0`)",
		store->my_section_my_arr_len,
		store->my_section_my_arr_len ? store->my_section_my_array[0] : ""

	);

	for (unsigned int idx = 1; idx < store->my_section_my_arr_len; idx++) {

		printf("|%s", store->my_section_my_array[idx]);

	}

	printf("]\n");

}

int main () {

	IniFormat my_format;

	struct ini_store my_store = { NULL, -1, FALSE, FALSE, NULL, 0, 0 };

	ini_set_implicit_value("YES", 0);
	my_format = INI_DEFAULT_FORMAT;
	my_format.semicolon = my_format.hash = INI_FORGET_COMMENT;
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

