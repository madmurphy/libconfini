/*  tests/performance/performance.c  */

#include <stdio.h>
#include <confini.h>
#include <time.h>

static int get_ini_size (IniStatistics * stats, void * v_bytes) {

	*((size_t *) v_bytes) = stats->bytes;

	return 0;

}

static int empty_listener (IniDispatch * dispatch, void * v_bytes) {

	return 0;

}

int main () {

	size_t bytes;
	double seconds;
	clock_t start, end;
	start = clock();

	/*  Please create an INI file large enough  */
	if (load_ini_path(
		"big_file.ini",
		INI_DEFAULT_FORMAT,
		get_ini_size,
		empty_listener,
		&bytes
	)) {

		return 1;

	}

	end = clock();
	seconds = (double) (end - start) / CLOCKS_PER_SEC;

	printf(
		"%zu bytes parsed in %f seconds.\n"
		"Number of bytes parsed per second: %f\n",
		bytes, seconds, bytes / seconds
	);

	return 0;

}

