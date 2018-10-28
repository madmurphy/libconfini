/*  examples/miscellanea/stats_only.c  */

#include <stdio.h>
#include <confini.h>

static int my_stats_handler (IniStatistics * statistics, void * user_data) {

	printf(
		"The file is %lu bytes large and contains %lu members.\n",
		statistics->bytes,
		statistics->members
	);

	return 0;

}

int main () {

	if (load_ini_path(
		"ini_files/example.conf",
		INI_DEFAULT_FORMAT,
		my_stats_handler,
		NULL,
		NULL
	)) {

		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}

	return 0;

}

