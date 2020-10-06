#include <iostream>
#include <confini.h>

using namespace std;

static int callback (IniDispatch * disp, void * v_other) {

	#define IS_KEY(SECTION, KEY) \
		(ini_array_match(SECTION, disp->append_to, '.', disp->format) && \
		ini_string_match_ii(KEY, disp->data, disp->format))

	if (disp->type == INI_KEY) {

		if (IS_KEY("users", "have_visited")) {

			/* No need to parse this field as an array right now */
			cout << "People who have visited: " << disp->value << "\n";

		} else if (IS_KEY("last_update", "date")) {

			cout << "Last update: " << disp->value << "\n";

		}

	}

	#undef IS_KEY

	return 0;

}

int main () {

	if (load_ini_path("../ini_files/log.ini", INI_DEFAULT_FORMAT, NULL, callback, NULL)) {

		cerr << "Sorry, something went wrong :-(\n";
		return 1;

	}

	return 0;

}

