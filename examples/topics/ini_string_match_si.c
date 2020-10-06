/*  examples/topics/ini_string_match_si.c  */

#include <stdio.h>
#include <confini.h>

static int passfinder (IniDispatch * disp, void * v_membid) {

  /*  Search for `password = "hello world"` in the INI file  */
  if (
    ini_string_match_si("password", disp->data, disp->format) &&
    ini_string_match_si("hello world", disp->value, disp->format)
  ) {

    *((size_t *) v_membid) = disp->dispatch_id;
    return 1;

  }

  return 0;

}

int main () {

  size_t membid;

  /*  Load INI file  */
  int retval = load_ini_path(
    "../ini_files/self_explaining.conf",
    INI_DEFAULT_FORMAT,
    NULL,
    passfinder,
    &membid
  );

  /*  Check for errors  */
  if (retval & CONFINI_ERROR) {

    fprintf(stderr, "Sorry, something went wrong :-(\n");
    return 1;

  }

  /*  Check if parsing has been interrupted by `passfinder()`  */
  retval  ==  CONFINI_FEINTR ?
                printf(
                  "We found it! It's the INI node number %zu!\n",
                  membid
                )
              :
                printf("We didn't find it :-(\n");

  return 0;

}

