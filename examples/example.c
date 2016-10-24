#include <stdio.h>
#include <confini.h>

#define FALSE 0
#define TRUE 1

struct my_struct {

	char *my_date;
	char *my_ini_file;

};

static int print_ini_stats (IniStatistics *statistics, void *user_data) {

	struct my_struct *my_other = (struct my_struct *) user_data;

	printf(

		"Hi there!\n\n"
		"Today is %s.\n\n"
		"We decided to parse the INI file \"%s\".\n\n"
		"The file is %d bytes large and contains %d nodes.\n"
		"Please find below the output of each dispatch.\n",

		my_other->my_date, my_other->my_ini_file, statistics->bytes, statistics->members

	);

	return 0;

}

static int my_ini_listener (IniDispatch *dispatch, void *user_data) {

	printf(

		"\ndispatch_id: %d\n"
		"format: {IniFormat}\n"
		"type: %d\n"
		"data: |%s|\n"
		"value: |%s|\n"
		"append_to: |%s|\n"
		"d_len: %d\n"
		"v_len: %d\n"
		"at_len: %d\n",

		dispatch->dispatch_id,
		dispatch->type,
		dispatch->data,
		dispatch->value,
		dispatch->append_to,
		dispatch->d_len,
		dispatch->v_len,
		dispatch->at_len

	);

	/* Check if this is an implicit key */

	if (dispatch->value == INI_IMPLICIT_VALUE) {

		printf("\n\tHey! This is an implicit value!\n");

	}

	return 0;

}

int main () {

	/* The format of the INI file */
	IniFormat my_format;

	/* User data */
	struct my_struct my_other;

	my_other.my_date = "Thursday September 22th, 2016";
	my_other.my_ini_file = "example.conf";

	/* Use the default format as model for the new format */
	my_format = INI_DEFAULT_FORMAT;

	/* Define the value to be shown in case of implicit keys, and its length */
	my_format.implicit_is_not_empty = TRUE;
	ini_set_implicit_value("YES", 0);

	/* Load INI file */
	if (load_ini_file("example.conf", my_format, print_ini_stats, my_ini_listener, &my_other)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	printf("\nFile \"%s\" has been parsed successfully.\n\n", my_other.my_ini_file);
	return 0;

}

