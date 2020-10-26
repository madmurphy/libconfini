/*  examples/miscellanea/toml-like.c  */
/*

  The following code will try to load a typed INI file that resembles the showcase
  TOML document found at https://github.com/toml-lang/toml

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <confini.h>

#define CLEAN_MALLOC(DEST, SIZE, RETVAL) \
	if (DEST) { free(DEST); } \
	DEST = malloc(SIZE); \
	if (!DEST) { \
		fprintf(stderr, "malloc() failed\n"); \
		return RETVAL; \
	}

struct Server_T {
	char * ip;
	char * dc;	
};

/*  This is the structure that maps our INI file  */
struct Configs_T {
	char * title;
	struct Owner_T {
		char * name;
		char * dob;
	} owner;
	struct Database_T {
		char * server;
		int * ports;
		size_t ports_length;
		int connection_max;
		bool enabled;
	} database;
	struct Servers_T {
		struct Server_T alpha;
		struct Server_T beta;
	} servers;
	struct Clients_T {
		/*
		  in the original INI file `servers.data` is an "array" containing
		  an "array" of strings followed by an "array" of integers. We
		  cannot do that in C, so we store the array of strings as
		  `servers.data_str` and the array of integer as `servers.data_num`
		*/
		char ** data_str;
		int * data_num;
		char ** hosts;
		size_t data_str_length;
		size_t data_num_length;
		size_t hosts_length;
	} clients;
};

/*  If `dest_str` is non-`NULL` free it, then `strdup(disp->value)` into it  */
int set_new_string (char ** dest_str, IniDispatch * const disp) {
	disp->d_len = ini_string_parse(disp->value, disp->format);
	CLEAN_MALLOC(*dest_str, disp->d_len, 1);
	memcpy(*dest_str, disp->value, disp->d_len + 1);
	return 0;
}

/*  If `dest_arr` is non-`NULL` free it, then parse `disp->value` into it as an array of strings  */
int set_new_strarray (char *** const dest_arr, size_t * const dest_len, const IniDispatch * const disp, const char delimiter) {
	*dest_len = ini_array_get_length(disp->value, delimiter, disp->format);
	CLEAN_MALLOC(*dest_arr, *dest_len * sizeof(char *) + disp->v_len + 1, 1);
	size_t idx = 0;
	char * token, * remnant = ((char *) *dest_arr) + (*dest_len + 1) * sizeof(char *);
	memcpy(remnant, disp->value == INI_GLOBAL_IMPLICIT_VALUE && !INI_GLOBAL_IMPLICIT_VALUE ? "" : disp->value, disp->v_len + 1);
	while ((token = ini_array_release(&remnant, delimiter, disp->format))) {
		ini_string_parse(token, disp->format);
		(*dest_arr)[idx++] = token;
	}
	return 0;
}

/*  If `dest_arr` is non-`NULL` free it, then parse `disp->value` into it as an array of integers  */
int set_new_intarray (int ** const dest_arr, size_t * const dest_len, const IniDispatch * const disp, const char delimiter) {
	*dest_len = ini_array_get_length(disp->value, delimiter, disp->format);
	CLEAN_MALLOC(*dest_arr, *dest_len * sizeof(int), 1);
	const char * shifted = disp->value;
	size_t idx = 0;
	while (shifted) {
		(*dest_arr)[idx++] = ini_get_int(shifted);
		ini_array_shift(&shifted, delimiter, disp->format);
	}
	return 0;
}

static int populate_config (IniDispatch * disp, void * v_confs) {
	#define confs ((struct Configs_T *) v_confs)
	if (disp->type == INI_KEY) {
		if (disp->at_len == 0) {
			if (ini_string_match_si("title", disp->data, disp->format)) {
				set_new_string(&confs->title, disp);
			}
		} else if (ini_array_match("owner", disp->append_to, '.', disp->format)) {
			if (ini_string_match_si("name", disp->data, disp->format)) {
				set_new_string(&confs->owner.name, disp);
			} else if (ini_string_match_si("dob", disp->data, disp->format)) {
				set_new_string(&confs->owner.dob, disp);
			}
		} else if (ini_array_match("database", disp->append_to, '.', disp->format)) {
			if (ini_string_match_si("server", disp->data, disp->format)) {
				set_new_string(&confs->database.server, disp);
			} else if (ini_string_match_si("ports", disp->data, disp->format)) {
				set_new_intarray(&confs->database.ports, &confs->database.ports_length, disp, ',');
			} else if (ini_string_match_si("connection_max", disp->data, disp->format)) {
				confs->database.connection_max = ini_get_int(disp->value);
			} else if (ini_string_match_si("enabled", disp->data, disp->format)) {
				confs->database.enabled = (bool) ini_get_bool_i(disp->value, 1, disp->format);
			}
		} else if (ini_array_match("servers.alpha", disp->append_to, '.', disp->format)) {
			if (ini_string_match_si("ip", disp->data, disp->format)) {
				set_new_string(&confs->servers.alpha.ip, disp);
			} else if (ini_string_match_si("dc", disp->data, disp->format)) {
				set_new_string(&confs->servers.alpha.dc, disp);
			}
		} else if (ini_array_match("servers.beta", disp->append_to, '.', disp->format)) {
			if (ini_string_match_si("ip", disp->data, disp->format)) {
				set_new_string(&confs->servers.beta.ip, disp);
			} else if (ini_string_match_si("dc", disp->data, disp->format)) {
				set_new_string(&confs->servers.beta.dc, disp);
			}
		} else if (ini_array_match("clients", disp->append_to, '.', disp->format)) {
			if (ini_string_match_si("data", disp->data, disp->format)) {
				const size_t arrlen = ini_array_get_length(disp->value, ',', disp->format);
				char * remaining = disp->value;
				if (arrlen > 0) {
					/*  array of strings `clients.data[0]` exists  */
					IniDispatch * dspclone = malloc(sizeof(IniDispatch));
					if (!dspclone) {
						fprintf(stderr, "malloc() failed\n");
						return 1;
					}
					memcpy(dspclone, disp, sizeof(IniDispatch));
					dspclone->value = ini_array_release(&remaining, ',', disp->format);
					dspclone->v_len = strlen(dspclone->value);
					set_new_strarray(&confs->clients.data_str, &confs->clients.data_str_length, dspclone, ':');
					if (arrlen > 1) {
						/*  array of integers `clients.data[1]` exists  */
						dspclone->value = ini_array_release(&remaining, ',', disp->format);
						dspclone->v_len = strlen(dspclone->value);
						set_new_intarray(&confs->clients.data_num, &confs->clients.data_num_length, dspclone, ':');
					}
					free(dspclone);
				}
			} else if (ini_string_match_si("hosts", disp->data, disp->format)) {
				set_new_strarray(&confs->clients.hosts, &confs->clients.hosts_length, disp, ',');
			}
		}
	}
	return 0;
	#undef confs
}
 
int main () {
	#define my_format \
		((IniFormat) { \
			.delimiter_symbol = INI_EQUALS, \
			.case_sensitive = false, \
			.semicolon_marker = INI_IGNORE, \
			.hash_marker = INI_IGNORE, \
			.section_paths = INI_ABSOLUTE_AND_RELATIVE, \
			.multiline_nodes = INI_MULTILINE_EVERYWHERE, \
			.no_single_quotes = false, \
			.no_double_quotes = false, \
			.no_spaces_in_names = false, \
			.implicit_is_not_empty = true, \
			.do_not_collapse_values = false, \
			.preserve_empty_quotes = false, \
			.disabled_after_space = false, \
			.disabled_can_be_implicit = true \
		})
	/*  **Important** The `confs` structure must be initialized with all its bytes set to `0`!  */
	struct Configs_T * confs = calloc(1, sizeof(struct Configs_T)); 
	if (!confs) { 
		fprintf(stderr, "malloc() failed\n");
		return 1; 
	}
	if (load_ini_path("../ini_files/toml-like.conf", my_format, NULL, populate_config, confs)) {
 		fprintf(stderr, "Sorry, something went wrong :-(\n");
		return 1;

	}
	/*  Print the stored configuration  */
	#define PRINT_CONF_ARRAY_WITHLABEL(PATH, LABEL, FORMAT) \
		for (size_t idx = 0; idx < confs->PATH##_length; idx++) { \
			if (confs->PATH) { printf(#LABEL "[%zu] -> " FORMAT "\n", idx, confs->PATH[idx]); } \
		}
	#define PRINT_CONF_ARRAY(PATH, FORMAT) \
		PRINT_CONF_ARRAY_WITHLABEL(PATH, PATH, FORMAT)
	#define PRINT_CONF_SIMPLEVAL(PATH, FORMAT) \
		if (confs->PATH) { printf(#PATH " -> " FORMAT "\n", confs->PATH); }
	PRINT_CONF_SIMPLEVAL(title, "%s");
	PRINT_CONF_SIMPLEVAL(owner.name, "%s");
	PRINT_CONF_SIMPLEVAL(owner.dob, "%s");
	PRINT_CONF_SIMPLEVAL(database.server, "%s");
	PRINT_CONF_ARRAY(database.ports, "%d");
	PRINT_CONF_SIMPLEVAL(database.connection_max, "%d");
	printf("database.enabled -> %s\n", confs->database.enabled ? "yes" : "no");
	PRINT_CONF_SIMPLEVAL(servers.alpha.ip, "%s");
	PRINT_CONF_SIMPLEVAL(servers.alpha.dc, "%s");
	PRINT_CONF_SIMPLEVAL(servers.beta.ip, "%s");
	PRINT_CONF_SIMPLEVAL(servers.beta.dc, "%s");
	PRINT_CONF_ARRAY_WITHLABEL(clients.data_str, clients.data[0], "%s");
	PRINT_CONF_ARRAY_WITHLABEL(clients.data_num, clients.data[1], "%d");
	PRINT_CONF_ARRAY(clients.hosts, "%s");
	/*  Free the allocated memory  */
	#define FREE_NONNULL(PTR) if (PTR) { free(PTR); }
	FREE_NONNULL(confs->title);
	FREE_NONNULL(confs->owner.name);
	FREE_NONNULL(confs->owner.dob);
	FREE_NONNULL(confs->database.server);
	FREE_NONNULL(confs->database.ports);
	FREE_NONNULL(confs->servers.alpha.ip);
	FREE_NONNULL(confs->servers.alpha.dc);
	FREE_NONNULL(confs->servers.beta.ip);
	FREE_NONNULL(confs->servers.beta.dc);
	FREE_NONNULL(confs->clients.data_str);
	FREE_NONNULL(confs->clients.data_num);
	FREE_NONNULL(confs->clients.hosts);
	free(confs);
	return 0;
}

